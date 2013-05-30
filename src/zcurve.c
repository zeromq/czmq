/*  =========================================================================
    zcurve - CurveZMQ security engine (rfc.zeromq.org/spec:26)

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the 
    Free Software Foundation; either version 3 of the License, or (at your 
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General 
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License 
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    CurveZMQ security engine, for use in ZeroMQ applications.
    This is a reference implementation of CurveZMQ, and can be used at the
    application level to secure a request-reply dialog (usually, DEALER to
    ROUTER). For an example of use, see the selftest function. To compile
    with security enabled, first build and install libsodium from GitHub at
    https://github.com/jedisct1/libsodium. Run ./configure after installing
    libsodium. If configure does not find libsodium, this class will work
    in clear text.
@discuss
    This class does no I/O; all socket input/output is done by the caller
    which passes frames to and from this class. It still lacks support for
    multipart messages, for client authentication (will be done using the
    rfc.zeromq.org/spec:27/ZAP protocol), and proper error handling.
@end
*/

#include "../include/czmq.h"
#if !defined (__WINDOWS__)
#   include "platform.h"
#endif

#if defined (HAVE_LIBSODIUM)
#   include <sodium.h>
#   if crypto_box_PUBLICKEYBYTES != 32 \
    || crypto_box_SECRETKEYBYTES != 32 \
    || crypto_box_BEFORENMBYTES != 32 \
    || crypto_box_ZEROBYTES != 32 \
    || crypto_box_BOXZEROBYTES != 16 \
    || crypto_box_NONCEBYTES != 24
#   error "libsodium not built correctly"
#   endif
#endif

typedef enum {
    pending,                    //  Waiting for first event
    expect_hello,               //  S: accepts HELLO from client
    expect_welcome,             //  C: accepts WELCOME from server
    expect_initiate,            //  S: accepts INITIATE from client
    expect_ready,               //  C: accepts READY from server
    connected                   //  C/S: accepts MESSAGE from server
} state_t;


//  Structure of our class
struct _zcurve_t {
    //  Long term keys, if known
    byte public_key [32];       //  Our long-term public key
    byte secret_key [32];       //  Our long-term secret key

    //  Server connection properties
    byte cookie_key [32];       //  Server cookie key
    byte cn_client [32];        //  Client's short-term public key
    byte client_key [32];       //  Client long-term public key

    //  Client connection properties
    byte server_key [32];       //  Server long-term public key
    byte cn_server [32];        //  Server's short-term public key
    byte cn_cookie [96];        //  Connection cookie from server

    //  Symmetric connection properties
    bool is_server;             //  True or false
    bool verbose;               //  Trace activity to stdout
    state_t state;              //  Connection state
    int64_t cn_nonce;           //  Connection nonce
    byte cn_public [32];        //  Connection public key
    byte cn_secret [32];        //  Connection secret key
    byte cn_precom [32];        //  Connection precomputed key

    //  Metadata properties
    byte metadata [1000];       //  Encoded for the wire
    size_t metadata_size;       //  Actual size so far
};

//  Command structures

typedef struct {
    byte id [8];                //  "HELLO   "
    byte version [2];           //  CurveZMQ major-minor version
    byte padding [70];          //  Anti-amplification padding
    byte client [32];           //  Client public connection key C'
    byte nonce [8];             //  Short nonce, prefixed by "CurveZMQHELLO---"
    byte box [80];              //  Signature, Box [64 * %x0](C'->S)
} hello_t;

typedef struct {
    byte id [8];                //  "WELCOME "
    byte nonce [16];            //  Long nonce, prefixed by "WELCOME-"
    byte box [144];             //  Box [S' + cookie](S->C')
} welcome_t;

typedef struct {
    byte id [8];                //  "INITIATE"
    byte cookie [96];           //  Server-provided cookie
    byte nonce [8];             //  Short nonce, prefixed by "CurveZMQINITIATE"
    byte box [112];             //  Box [C + vouch + metadata](C'->S')
} initiate_t;

typedef struct {
    byte id [8];                //  "READY   "
    byte nonce [8];             //  Short nonce, prefixed by "CurveZMQREADY---"
    byte box [16];              //  Box [metadata](S'->C')
} ready_t;

typedef struct {
    byte id [8];                //  "MESSAGE "
    byte nonce [8];             //  Short nonce, prefixed by "CurveZMQMESSAGE-"
    byte box [16];              //  Box [payload](S'->C') or (C'->S')
} message_t;


//  --------------------------------------------------------------------------
//  Create a new zcurve instance; if you provide a server-key, is a client
//  that can talk to that specific server. Otherwise is a server that will
//  talk to one specific client.

zcurve_t *
zcurve_new (byte *server_key)
{
    zcurve_t *self = (zcurve_t *) zmalloc (sizeof (zcurve_t));
    assert (self);
    if (server_key) {
        memcpy (self->server_key, server_key, 32);
        self->is_server = false;
        self->state = pending;
    }
    else {
        self->is_server = true;
        self->state = expect_hello;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zcurve_destroy (zcurve_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zcurve_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Generate a new long-term key pair

void
zcurve_keypair_new (zcurve_t *self)
{
    assert (self);
#if defined (HAVE_LIBSODIUM)
    int rc = crypto_box_keypair (self->public_key, self->secret_key);
    assert (rc == 0);
#endif
}


//  Return allocated string containing key in printable hex format

char *s_key_to_hex (byte *key)
{
    char *hex = zmalloc (65);
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < 32; byte_nbr++) 
        sprintf (hex + (byte_nbr * 2), "%02X", key [byte_nbr]);
    return hex;
}


//  --------------------------------------------------------------------------
//  Save long-term key pair to disk; not confidential

int
zcurve_keypair_save (zcurve_t *self)
{
    assert (self);

    //  Get printable key strings
    char *public_key = s_key_to_hex (self->public_key);
    char *secret_key = s_key_to_hex (self->secret_key);
    
    //  Set process file create mask to owner access only
    zfile_mode_private ();
    
    //  The public key file contains just the public key
    zconfig_t *root = zconfig_new ("root", NULL);
    zconfig_t *key = zconfig_new ("public-key", root);
    zconfig_set_value (key, public_key);
    zconfig_save (root, "public.key");
    
    //  The secret key file contains both secret and public keys
    key = zconfig_new ("secret-key", root);
    zconfig_set_value (key, secret_key);
    zconfig_save (root, "secret.key");
    zconfig_destroy (&root);
    
    //  Reset process file create mask
    zfile_mode_default ();
    
    free (public_key);
    free (secret_key);
    return 0;
}


//  --------------------------------------------------------------------------
//  Load long-term key pair from disk; returns 0 if OK, -1 if error

int
zcurve_keypair_load (zcurve_t *self)
{
    assert (self);
    zconfig_t *root = zconfig_load ("secret.key");
    assert (root);
    int rc = 0;
    char *secret_key = zconfig_resolve (root, "secret-key", NULL);
    char *public_key = zconfig_resolve (root, "public-key", NULL);
    if (secret_key && public_key) {
        int byte_nbr, matches = 0;
        for (byte_nbr = 0; byte_nbr < 32; byte_nbr++) {
            matches += sscanf (secret_key + byte_nbr * 2, "%02hhX ", &self->secret_key [byte_nbr]);
            matches += sscanf (public_key + byte_nbr * 2, "%02hhX ", &self->public_key [byte_nbr]);
        }
        //  TODO: don't assert, but raise error exception
        assert (matches == 64);
    }
    else
        rc = -1;

    zconfig_destroy (&root);
    return rc;
}


//  --------------------------------------------------------------------------
//  Return public part of key pair

byte *
zcurve_keypair_public (zcurve_t *self)
{
    assert (self);
    return self->public_key;
}


//  --------------------------------------------------------------------------
//  Set a metadata property; these are sent to the peer after the security 
//  handshake. Property values are strings.

void
zcurve_set_metadata (zcurve_t *self, char *name, char *value)
{
    assert (self);
    assert (name && value);
    size_t name_len = strlen (name);
    size_t value_len = strlen (value);
    assert (name_len > 0 && name_len < 256);
    byte *needle = self->metadata + self->metadata_size;

    //  Encode name
    *needle++ = (byte) name_len;
    memcpy (needle, name, name_len);
    needle += name_len;

    //  Encode value
    *needle++ = (byte) ((value_len >> 24) && 255);
    *needle++ = (byte) ((value_len >> 16) && 255);
    *needle++ = (byte) ((value_len >> 8)  && 255);
    *needle++ = (byte) ((value_len)       && 255);
    memcpy (needle, value, value_len);
    needle += value_len;

    //  Update size of metadata so far
    self->metadata_size = needle - self->metadata;
}


//  --------------------------------------------------------------------------
//  Set tracing on zcurve instance. Will report activity to stdout.

void
zcurve_set_verbose (zcurve_t *self, bool verbose)
{
    assert (self);
    self->verbose = verbose;
}


//  --------------------------------------------------------------------------
//  Internal functions for working with CurveZMQ commands

//  Encrypt a block of data using the connection nonce
//  If key_to/key_from are null, uses precomputed key
    
static void
s_encrypt (
    zcurve_t *self,         //  zcurve instance sending the data
    byte *target,           //  target must be nonce + box
    byte *data,             //  Clear text data to encrypt
    size_t size,            //  Size of clear text data
    char *prefix,           //  Nonce prefix to use, 8 or 16 chars
    byte *key_to,           //  Key to encrypt to, may be null
    byte *key_from)         //  Key to encrypt from, may be null
{
#if defined (HAVE_LIBSODIUM)
    size_t box_size = crypto_box_ZEROBYTES + size;
    byte *plain = malloc (box_size);
    byte *box = malloc (box_size);

    //  Prepare plain text with zero bytes at start for encryption
    memset (plain, 0, crypto_box_ZEROBYTES);
    memcpy (plain + crypto_box_ZEROBYTES, data, size);

    //  Prepare full nonce and store nonce into target
    //  Handle both short and long nonces
    byte nonce [24];
    if (strlen (prefix) == 16) {
        //  Long nonce is sequential integer
        memcpy (nonce, (byte *) prefix, 16);
        memcpy (nonce + 16, &self->cn_nonce, 8);
        memcpy (target, &self->cn_nonce, 8);
        self->cn_nonce++;
        target += 8;            //  Encrypted data comes after 8 byte nonce
    }
    else {
        //  Short nonce is random sequence
        randombytes (target, 16);
        memcpy (nonce, (byte *) prefix, 8);
        memcpy (nonce + 8, target, 16);
        target += 16;           //  Encrypted data comes after 16 byte nonce
    }
    //  Create box using either key pair, or precomputed key
    int rc;
    if (key_to)
        rc = crypto_box (box, plain, box_size, nonce, key_to, key_from);
    else
        rc = crypto_box_afternm (box, plain, box_size, nonce, self->cn_precom);
    assert (rc == 0);
    
    memcpy (target, box + crypto_box_BOXZEROBYTES, size + crypto_box_BOXZEROBYTES);
    free (plain);
    free (box);
#else
    //  If not built with crypto, store clear text
    memcpy (target, data, size);
#endif
}


//  Decrypt a block of data using the connection nonce and precomputed key
//  If key_to/key_from are null, uses precomputed key
    
static void
s_decrypt (
    zcurve_t *self,         //  zcurve instance sending the data
    byte *source,           //  source must be nonce + box
    byte *data,             //  Where to store decrypted clear text
    size_t size,            //  Size of clear text data
    char *prefix,           //  Nonce prefix to use, 8 or 16 chars
    byte *key_to,           //  Key to decrypt to, may be null
    byte *key_from)         //  Key to decrypt from, may be null
{
#if defined (HAVE_LIBSODIUM)
    size_t box_size = crypto_box_ZEROBYTES + size;
    byte *plain = malloc (box_size);
    byte *box = malloc (box_size);

    //  Prepare the full nonce from prefix and source
    //  Handle both short and long nonces
    byte nonce [24];
    if (strlen (prefix) == 16) {
        memcpy (nonce, (byte *) prefix, 16);
        memcpy (nonce + 16, source, 8);
        source += 8;
    }
    else {
        memcpy (nonce, (byte *) prefix, 8);
        memcpy (nonce + 8, source, 16);
        source += 16;
    }
    //  Get encrypted box from source
    memset (box, 0, crypto_box_BOXZEROBYTES);
    memcpy (box + crypto_box_BOXZEROBYTES, source, size + crypto_box_BOXZEROBYTES);
    
    //  Open box using either key pair, or precomputed key
    int rc;
    if (key_to)
        rc = crypto_box_open (plain, box, box_size, nonce, key_to, key_from);
    else
        rc = crypto_box_open_afternm (plain, box, box_size, nonce, self->cn_precom);
    //  TODO: don't assert, but raise error on connection
    assert (rc == 0);
    
    memcpy (data, plain + crypto_box_ZEROBYTES, size);
    free (plain);
    free (box);
#else
    //  If not built with crypto, use clear text
    memcpy (data, source, size);
#endif
}

static zframe_t *
s_produce_hello (zcurve_t *self)
{
    zframe_t *command = zframe_new (NULL, sizeof (hello_t));
    hello_t *hello = (hello_t *) zframe_data (command);
    memcpy (hello->id, "HELLO   ", 8);

#if defined (HAVE_LIBSODIUM)
    //  Generate connection key pair
    int rc = crypto_box_keypair (self->cn_public, self->cn_secret);
    assert (rc == 0);
#endif
    memcpy (hello->client, self->cn_public, 32);
    byte signature [64] = { 0 };
    s_encrypt (self, hello->nonce, 
               signature, 64,
               "CurveZMQHELLO---", 
               self->server_key, self->cn_secret);
    return command;
}

static void
s_process_hello (zcurve_t *self, zframe_t *input)
{
    if (self->verbose)
        printf ("\nC:HELLO: ");
    hello_t *hello = (hello_t *) zframe_data (input);

    memcpy (self->cn_client, hello->client, 32);
    byte signature [64];
    s_decrypt (self, hello->nonce, 
               signature, 64, 
               "CurveZMQHELLO---", 
               hello->client, self->secret_key);
    if (self->verbose)
        puts ("OK");
}

static zframe_t *
s_produce_welcome (zcurve_t *self)
{
    zframe_t *command = zframe_new (NULL, sizeof (welcome_t));
    welcome_t *welcome = (welcome_t *) zframe_data (command);
    memcpy (welcome->id, "WELCOME ", 8);

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte plain [256];

    //  Generate connection key pair
    int rc = crypto_box_keypair (self->cn_public, self->cn_secret);
    assert (rc == 0);

    //  Generate cookie = Box [C' + s'](t),
    memset (plain, 0, crypto_box_ZEROBYTES);
    memcpy (plain + crypto_box_ZEROBYTES, self->cn_client, 32);
    memcpy (plain + crypto_box_ZEROBYTES + 32, self->cn_secret, 32);

    //  Create full nonce for encryption
    //  8-byte prefix plus 16-byte random nonce
    assert (crypto_box_BOXZEROBYTES == 16);
    byte cookie_nonce [16];
    randombytes (cookie_nonce, 16);
    memcpy (nonce, (byte *) "COOKIE--", 8);
    memcpy (nonce + 8, cookie_nonce, 16);

    //  Encrypt using one-time symmetric cookie key
    randombytes (self->cookie_key, 32);
    byte cookie_box [96];
    rc = crypto_secretbox (cookie_box, plain, 96, nonce, self->cookie_key);
    assert (rc == 0);

    //  Create Box [S' + cookie](S->C')
    memcpy (plain, self->cn_public, 32);
    memcpy (plain + 32, cookie_nonce, 16);
    memcpy (plain + 48, cookie_box + crypto_box_BOXZEROBYTES, 80);
    s_encrypt (self, welcome->nonce, 
               plain, 128, "WELCOME-", 
               self->cn_client, self->secret_key);
#endif
    return command;
}

static void
s_process_welcome (zcurve_t *self, zframe_t *input)
{
    if (self->verbose)
        printf ("S:WELCOME: ");

#if defined (HAVE_LIBSODIUM)
    //  Open Box [S' + cookie](C'->S)
    byte plain [128];
    welcome_t *welcome = (welcome_t *) zframe_data (input);
    s_decrypt (self, welcome->nonce, 
               plain, 128, "WELCOME-", 
               self->server_key, self->cn_secret);
    memcpy (self->cn_server, plain, 32);
    memcpy (self->cn_cookie, plain + 32, 96);
    
    //  Pre-compute connection secret from server key
    int rc = crypto_box_beforenm (self->cn_precom, self->cn_server, self->cn_secret);
    assert (rc == 0);
#endif
    if (self->verbose)
        puts ("OK");
}

static zframe_t *
s_produce_initiate (zcurve_t *self)
{
    zframe_t *command = zframe_new (NULL, sizeof (initiate_t) + self->metadata_size);
    initiate_t *initiate = (initiate_t *) zframe_data (command);
    memcpy (initiate->id, "INITIATE", 8);
    memcpy (initiate->cookie, self->cn_cookie, sizeof (initiate->cookie));

#if defined (HAVE_LIBSODIUM)
    //  Create vouch = Box [C'](C->S)
    byte vouch [64];
    s_encrypt (self, vouch, 
               self->cn_public, 32, "VOUCH---", 
               self->server_key, self->secret_key);
    
    //  Working variables for crypto calls
    size_t box_size = 96 + self->metadata_size;
    byte *plain = malloc (box_size);
    byte *box = malloc (box_size);

    //  Create Box [C + vouch + metadata](C'->S')
    memcpy (plain, self->public_key, 32);
    memcpy (plain + 32, vouch, 64);
    memcpy (plain + 96, self->metadata, self->metadata_size);
    s_encrypt (self, initiate->nonce, 
               plain, 96 + self->metadata_size,
               "CurveZMQINITIATE", 
               self->cn_server, self->cn_secret);
    free (plain);
    free (box);
#endif
    return command;
}

static void
s_process_initiate (zcurve_t *self, zframe_t *input)
{
    if (self->verbose)
        printf ("C:INITIATE: ");

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    
    initiate_t *initiate = (initiate_t *) zframe_data (input);
    size_t metadata_size = zframe_size (input) - sizeof (initiate_t);
    size_t box_size = crypto_box_ZEROBYTES + 96 + metadata_size;
    byte *plain = malloc (box_size);
    byte *box = malloc (box_size);

    //  Check cookie is valid
    //  We could but don't expire cookie key after 60 seconds
    //  Cookie nonce is first 16 bytes of cookie
    memcpy (nonce, (byte *) "COOKIE--", 8);
    memcpy (nonce + 8, initiate->cookie, 16);
    //  Cookie box is next 80 bytes of cookie
    memset (box, 0, crypto_box_BOXZEROBYTES);
    memcpy (box + crypto_box_BOXZEROBYTES, initiate->cookie + 16, 80);
    int rc = crypto_secretbox_open (plain, box,
                                    crypto_box_BOXZEROBYTES + 80,
                                    nonce, self->cookie_key);
    //  Throw away cookie key
    memset (self->cookie_key, 0, 32);

    //  Check cookie plain text is as expected [C' + s']
    //  TODO: don't assert, but raise error on connection
    assert (memcmp (plain + crypto_box_ZEROBYTES, self->cn_client, 32) == 0);
    assert (memcmp (plain + crypto_box_ZEROBYTES + 32, self->cn_secret, 32) == 0);

    //  Open Box [C + vouch + metadata](C'->S')
    s_decrypt (self, initiate->nonce, 
               plain, 96 + metadata_size, 
               "CurveZMQINITIATE", 
               self->cn_client, self->cn_secret);
    
    //  This is where we'd check the decrypted client public key
    memcpy (self->client_key, plain, 32);
    //  Metadata is at plain + 96
    if (self->verbose)
        printf ("(received %zd bytes metadata) ", metadata_size);
    //  Vouch nonce + box is 64 bytes at plain + 32
    byte vouch [64];
    memcpy (vouch, plain + 32, 64);
    s_decrypt (self, vouch, 
               plain, 32, "VOUCH---",
               self->client_key, self->secret_key);
    
    //  What we decrypted must be the short term client public key
    //  TODO: don't assert, but raise error on connection
    assert (memcmp (plain, self->cn_client, 32) == 0);

    //  Precompute connection secret from client key
    rc = crypto_box_beforenm (self->cn_precom, self->cn_client, self->cn_secret);
    assert (rc == 0);
    
    free (plain);
    free (box);
#endif
    if (self->verbose)
        puts ("OK");
}

static zframe_t *
s_produce_ready (zcurve_t *self)
{
    zframe_t *command = zframe_new (NULL, sizeof (ready_t) + self->metadata_size);
    ready_t *ready = (ready_t *) zframe_data (command);
    memcpy (ready->id, "READY   ", 8);
    s_encrypt (self, ready->nonce, 
               self->metadata, self->metadata_size, 
               "CurveZMQREADY---", 
               NULL, NULL);
    return command;
}

static void
s_process_ready (zcurve_t *self, zframe_t *input)
{
    ready_t *ready = (ready_t *) zframe_data (input);
    if (self->verbose)
        printf ("C:READY: ");
    self->metadata_size = zframe_size (input) - sizeof (ready_t);
    s_decrypt (self, ready->nonce, 
               self->metadata, self->metadata_size, 
               "CurveZMQREADY---", 
               NULL, NULL);
    if (self->verbose)
        printf ("(received %zd bytes metadata) OK\n", self->metadata_size);
}

static zframe_t *
s_produce_message (zcurve_t *self, zframe_t *clear)
{
    zframe_t *command = zframe_new (NULL, sizeof (message_t) + zframe_size (clear));
    message_t *message = (message_t *) zframe_data (command);
    memcpy (message->id, "MESSAGE ", 8);
    s_encrypt (self, message->nonce, 
               zframe_data (clear), zframe_size (clear), 
               self->is_server? "CurveZMQMESSAGES": "CurveZMQMESSAGEC", 
               NULL, NULL);
    return command;
}

static zframe_t *
s_process_message (zcurve_t *self, zframe_t *input)
{
    message_t *message = (message_t *) zframe_data (input);
    if (self->verbose)
        printf ("%c:MESSAGE: ", self->is_server? 'C': 'S');
    
    size_t size = zframe_size (input) - sizeof (message_t);
    zframe_t *output = zframe_new (NULL, size);
    s_decrypt (self, message->nonce, 
               zframe_data (output), size, 
               self->is_server? "CurveZMQMESSAGEC": "CurveZMQMESSAGES", 
               NULL, NULL);
    if (self->verbose)
        printf ("(received %zd bytes data) OK\n", size);
    return output;
}


//  --------------------------------------------------------------------------
//  Accept input command from peer. If the command is invalid, it is
//  discarded silently. May return a frame to send to the peer, or NULL
//  if there is nothing to send.

zframe_t *
zcurve_execute (zcurve_t *self, zframe_t *input)
{
    assert (self);
    
    //  Pending state - ignore input
    if (self->state == pending) {
        self->state = expect_welcome;
        return s_produce_hello (self);
    }
    //  All other states require input
    assert (input);
    size_t size = zframe_size (input);
    byte *data = zframe_data (input);
    zframe_t *output = NULL;

    if (self->state == expect_hello
    &&  size == sizeof (hello_t)
    &&  memcmp (data, "HELLO   ", 8) == 0) {
        s_process_hello (self, input);
        output = s_produce_welcome (self);
        self->state = expect_initiate;
    }
    else
    if (self->state == expect_welcome
    &&  size == sizeof (welcome_t)
    &&  memcmp (data, "WELCOME ", 8) == 0) {
        s_process_welcome (self, input);
        output = s_produce_initiate (self);
        self->state = expect_ready;
    }
    else
    if (self->state == expect_initiate
    &&  size >= sizeof (initiate_t)
    &&  memcmp (data, "INITIATE ", 8) == 0) {
        s_process_initiate (self, input);
        output = s_produce_ready (self);
        self->state = connected;
    }
    else
    if (self->state == expect_ready
    &&  size >= sizeof (ready_t)
    &&  memcmp (data, "READY   ", 8) == 0) {
        s_process_ready (self, input);
        self->state = connected;
    }
    else {
        if (self->verbose)
            puts ("E: invalid command");
        assert (false);
    }
    return output;
}


//  --------------------------------------------------------------------------
//  Encode clear-text message to peer. Returns a frame ready to send
//  on the wire. Takes ownership of clear-text frame.

zframe_t *
zcurve_encode (zcurve_t *self, zframe_t **cleartext_p)
{
    assert (self);
    assert (self->state == connected);
    assert (cleartext_p);
    assert (*cleartext_p);
    
    zframe_t *encrypted = s_produce_message (self, *cleartext_p);
    zframe_destroy (cleartext_p);
    return encrypted;
}


//  --------------------------------------------------------------------------
//  Decode blob into message from peer. Takes ownership of encrypted frame.

zframe_t *
zcurve_decode (zcurve_t *self, zframe_t **encrypted_p)
{
    assert (self);
    assert (self->state == connected);
    assert (encrypted_p);
    assert (*encrypted_p);
    
    zframe_t *cleartext = NULL;
    if (zframe_size (*encrypted_p) >= sizeof (message_t)
    &&  memcmp (zframe_data (*encrypted_p), "MESSAGE ", 8) == 0)
        cleartext = s_process_message (self, *encrypted_p);
    else
    if (self->verbose)
        puts ("E: invalid command");
    
    zframe_destroy (encrypted_p);
    return cleartext;
}


//  --------------------------------------------------------------------------
//  Indicate whether handshake is still in progress

bool
zcurve_connected (zcurve_t *self)
{
    assert (self);
    return (self->state == connected);
}


//  --------------------------------------------------------------------------
//  Selftest

//  @selftest
void *
server_task (void *args)
{
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *router = zsocket_new (ctx, ZMQ_ROUTER);
    int rc = zsocket_bind (router, "tcp://*:9000");
    assert (rc != -1);

    //  Create a new server instance and load its keys from the previously 
    //  generated keypair file
    zcurve_t *server = zcurve_new (NULL);
    rc = zcurve_keypair_load (server);
    assert (rc == 0);

    //  Set some metadata properties
    zcurve_set_metadata (server, "Server", "CZMQ/zcurve");
    
    //  A hack to get the thread to timeout and exit so we can test
    //  under Valgrind. Do NOT do this on real servers!
    zsocket_set_rcvtimeo (router, 1000);

    //  Execute incoming frames until ready or exception
    //  In practice we'd want a server instance per unique client
    while (!zcurve_connected (server)) {
        zframe_t *sender = zframe_recv (router);
        zframe_t *input = zframe_recv (router);
        assert (input);
        zframe_t *output = zcurve_execute (server, input);
        zframe_destroy (&input);
        zframe_send (&sender, router, ZFRAME_MORE);
        zframe_send (&output, router, 0);
    }
    while (true) {
        //  Now act as echo service doing a full decode and encode
        //  Finish when we get an END message
        zframe_t *sender = zframe_recv (router);
        if (!sender)
            break;          //  Timed-out, finished
        zframe_t *encrypted = zframe_recv (router);
        assert (encrypted);
        zframe_t *cleartext = zcurve_decode (server, &encrypted);
        assert (cleartext);
        
        encrypted = zcurve_encode (server, &cleartext);
        assert (encrypted);
        zframe_send (&sender, router, ZFRAME_MORE);
        zframe_send (&encrypted, router, 0);
    }
    zcurve_destroy (&server);
    zctx_destroy (&ctx);
    return NULL;
}
//  @end

void
zcurve_test (bool verbose)
{
    printf (" * zcurve: ");

    //  @selftest
    //  Generate new long-term key pair for our test server
    //  The key pair will be stored in "secret.key"
    
    zcurve_t *keygen = zcurve_new (NULL);
    zcurve_keypair_new (keygen);
    int rc = zcurve_keypair_save (keygen);
    assert (rc == 0);
    assert (zfile_exists ("secret.key"));
    
    //  This is how we "share" the server key in our test
    byte server_key [32];
    memcpy (server_key, zcurve_keypair_public (keygen), 32);
    zcurve_destroy (&keygen);
    
    //  We'll run the server as a background task, and the
    //  client in this foreground thread.
    zthread_new (server_task, NULL);

    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *dealer = zsocket_new (ctx, ZMQ_DEALER);
    rc = zsocket_connect (dealer, "tcp://127.0.0.1:9000");
    assert (rc != -1);
    
    //  Create a new client instance using shared server key
    zcurve_t *client = zcurve_new (server_key);
    zcurve_set_verbose (client, verbose);
    zcurve_keypair_new (client);

    //  Set some metadata properties
    zcurve_set_metadata (client, "Client", "CZMQ/zcurve");
    zcurve_set_metadata (client, "Identity", "E475DA11");
    
    //  Execute null event on client to kick off handshake
    zframe_t *output = zcurve_execute (client, NULL);
    while (!zcurve_connected (client)) {
        rc = zframe_send (&output, dealer, 0);
        assert (rc >= 0);
        zframe_t *input = zframe_recv (dealer);
        assert (input);
        output = zcurve_execute (client, input);
        zframe_destroy (&input);
    }
    //  Handshake is done, now try Hello, World
    zframe_t *cleartext = zframe_new ((byte *) "Hello, World", 12);
    zframe_t *encrypted = zcurve_encode (client, &cleartext);
    assert (encrypted);
    zframe_send (&encrypted, dealer, 0);

    encrypted = zframe_recv (dealer);
    assert (encrypted);
    
    cleartext = zcurve_decode (client, &encrypted);
    assert (cleartext);
    assert (zframe_size (cleartext) == 12);
    assert (memcmp (zframe_data (cleartext), "Hello, World", 12) == 0);
    zframe_destroy (&cleartext);
    
    //  Now send messages of increasing size, check they work
    int count;
    int size = 0;
    for (count = 0; count < 18; count++) {
        if (verbose)
            printf ("Testing message of size=%d...\n", size);
        cleartext = zframe_new (NULL, size);
        int byte_nbr;
        //  Set data to sequence 0...255 repeated
        for (byte_nbr = 0; byte_nbr < size; byte_nbr++) 
            zframe_data (cleartext)[byte_nbr] = (byte) byte_nbr;

        encrypted = zcurve_encode (client, &cleartext);
        assert (encrypted);
        zframe_send (&encrypted, dealer, 0);
        
        encrypted = zframe_recv (dealer);
        assert (encrypted);
        cleartext = zcurve_decode (client, &encrypted);
        assert (cleartext);
        assert (zframe_size (cleartext) == size);
        for (byte_nbr = 0; byte_nbr < size; byte_nbr++) {
            assert (zframe_data (cleartext)[byte_nbr] == (byte) byte_nbr);
        }
        zframe_destroy (&cleartext);
        size = size * 2 + 1;
    }
    //  Give server thread a chance to time-out and exit
    zclock_sleep (1000);

    //  Done, clean-up
    zfile_delete ("public.key");
    zfile_delete ("secret.key");
    zcurve_destroy (&client);
    zctx_destroy (&ctx);
    //  @end
    
    printf ("OK\n");
}
