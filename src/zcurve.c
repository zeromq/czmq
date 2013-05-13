/*  =========================================================================
    zcurve - CurveZMQ security engine (rfc.zeromq.org/spec:26)

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    CurveZMQ security engine, for use in ZeroMQ applications.
@discuss
    This is a reference implementation of CurveZMQ, and can be used at the
    application level to secure a request-reply dialog (usually, DEALER to
    ROUTER). For an example of use, see the selftest function.
@end
*/

#include "../include/czmq.h"
// Only include platform.h if we're not building using visual studio
#if !defined(_MSC_VER)
#include "platform.h"
#endif

#undef HAVE_LIBSODIUM

#if defined (HAVE_LIBSODIUM)
#   include <sodium.h>
#   if crypto_box_PUBLICKEYBYTES != 32 \
    || crypto_box_SECRETKEYBYTES != 32 \
    || crypto_box_BEFORENMBYTES != 32
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
    state_t state;              //  Connection state
    int64_t cn_nonce;           //  Connection nonce
    byte cn_public [32];        //  Connection public key
    byte cn_secret [32];        //  Connection secret key
    byte cn_precom [32];        //  Connection precomputed key
    zmq_msg_t msg;              //  ZMQ message to send to peer
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
    byte box [112];                //  Box [C + vouch + metadata](C'->S')
} initiate_t;

typedef struct {
    byte id [8];                //  "READY   "
    byte nonce [8];             //  Short nonce, prefixed by "CurveZMQREADY---"
    byte box [116];                //  Box [metadata](S'->C')
} ready_t;

typedef struct {
    byte id [8];                //  "MESSAGE "
    byte nonce [8];             //  Short nonce, prefixed by "CurveZMQMESSAGE-"
    byte box [116];                //  Box [payload](S'->C') or (C'->S')
} message_t;


//  --------------------------------------------------------------------------
//  Constructor. Always generates a new public/secret key pair which
//  you can override if you need to.

zcurve_t *
zcurve_new (byte *server_key)
{
    zcurve_t
        *self;

    self = (zcurve_t *) zmalloc (sizeof (zcurve_t));
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
        zmq_msg_close (&self->msg);
        free (self);
        *self_p = NULL;
    }
}


void
s_dump (byte *buffer, int size, char *prefix)
{
    printf ("\n%s: ", prefix);
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < size; byte_nbr++)
        printf ("%02X ", buffer [byte_nbr]);
    printf ("\n");
}

//  --------------------------------------------------------------------------
//  Generate a new long-term key pair

CZMQ_EXPORT void
zcurve_keypair_new (zcurve_t *self)
{
    assert (self);
#if defined (HAVE_LIBSODIUM)
    int rc = crypto_box_keypair (self->public_key, self->secret_key);
    assert (rc == 0);
#endif
}


//  --------------------------------------------------------------------------
//  Save long-term key pair to disk; not confidential

CZMQ_EXPORT int
zcurve_keypair_save (zcurve_t *self)
{
#   define PUBKEY_FILE "public.key"
#   define PUBKEY_OPEN  "-----BEGIN ZCURVE PUBLIC KEY-----\n"
#   define PUBKEY_CLOSE "-----END ZCURVE PUBLIC KEY-----\n"
#   define SECKEY_FILE "secret.key"
#   define SECKEY_OPEN  "-----BEGIN ZCURVE SECRET KEY-----\n"
#   define SECKEY_CLOSE "-----END ZCURVE SECRET KEY-----\n"

    assert (self);
    //  Set process file create mask to owner access only
#   if !defined(__WINDOWS__)
	mode_t old_mask = umask (S_IWGRP | S_IWOTH | S_IRGRP | S_IROTH);
#   endif
	//  The public key file contains just the public keys
    FILE *file = fopen (PUBKEY_FILE, "w");
    //  Reset process file create mask
#   if !defined(__WINDOWS__)
	umask (old_mask);
#   endif

    if (!file)
        return -1;
    
    fprintf (file, PUBKEY_OPEN);
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < 32; byte_nbr++) 
        fprintf (file, "%02X", self->public_key [byte_nbr]);
    fprintf (file, "\n");
    fprintf (file, PUBKEY_CLOSE);
    fclose (file);

    //  The secret key file contains both public and secret keys
    file = fopen (SECKEY_FILE, "w");
    fprintf (file, SECKEY_OPEN);
    for (byte_nbr = 0; byte_nbr < 32; byte_nbr++)
        fprintf (file, "%02X", self->public_key [byte_nbr]);
    fprintf (file, "\n");
    for (byte_nbr = 0; byte_nbr < 32; byte_nbr++)
        fprintf (file, "%02X", self->secret_key [byte_nbr]);
    fprintf (file, "\n");
    fprintf (file, SECKEY_CLOSE);
    fclose (file);

    return 0;
}


//  --------------------------------------------------------------------------
//  Load long-term key pair from disk; returns 0 if OK, -1 if error

CZMQ_EXPORT int
zcurve_keypair_load (zcurve_t *self)
{
    assert (self);
    
    FILE *file = fopen (SECKEY_FILE, "r");
    if (!file)
        return -1;

    char buffer [256];
    int matches = 0;
    if (fgets (buffer, 256, file)
    &&  streq (buffer, SECKEY_OPEN)) {
        int byte_nbr;
        for (byte_nbr = 0; byte_nbr < 32; byte_nbr++)
            matches += fscanf (file, "%02hhX ", &self->public_key [byte_nbr]);
        for (byte_nbr = 0; byte_nbr < 32; byte_nbr++)
            matches += fscanf (file, "%02hhX ", &self->secret_key [byte_nbr]);
    }
    fclose (file);
    return matches == 64? 0: -1;
}


//  --------------------------------------------------------------------------
//  Return public part of key pair

CZMQ_EXPORT byte *
zcurve_keypair_public (zcurve_t *self)
{
    assert (self);
    return self->public_key;
}


static zmq_msg_t *
s_produce_hello (zcurve_t *self)
{
    zmq_msg_close (&self->msg);
    zmq_msg_init_size (&self->msg, sizeof (hello_t));
    hello_t *hello = (hello_t *) zmq_msg_data (&self->msg);
    memcpy (hello->id, "HELLO   ", 8);
    
#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
    byte plain [256];

    //  Generate connection key pair
    int rc = crypto_box_keypair (self->cn_public, self->cn_secret);
    assert (rc == 0);
    
    //  Create Box [64 * %x0](C'->S)
    assert (crypto_box_ZEROBYTES == 32);
    memset (plain, 0, crypto_box_ZEROBYTES + 64);

    //  Prepare the full nonce and encrypt
    assert (crypto_box_NONCEBYTES == 24);
    memcpy (nonce, (byte *) "CurveZMQHELLO---", 16);
    memcpy (nonce + 16, &self->cn_nonce, 8);
    rc = crypto_box (box, plain, crypto_box_ZEROBYTES + 64,
        nonce, self->server_key, self->cn_secret);
    assert (rc == 0);

    memcpy (hello->client, self->cn_public, 32);
    memcpy (hello->nonce, &self->cn_nonce, 8);
    memcpy (hello->box, box + crypto_box_BOXZEROBYTES, sizeof (hello->box));
    self->cn_nonce++;
#endif
    return &self->msg;
}

static void
s_process_hello (zcurve_t *self, hello_t *hello)
{
    printf ("C:HELLO: ");

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
    byte plain [256];

    //  Open Box [64 * %x0](C'->S)
    memset (box, 0, crypto_box_BOXZEROBYTES);
    memcpy (box + crypto_box_BOXZEROBYTES, hello->box, sizeof (hello->box));

    //  Prepare the full nonce and decrypt
    memcpy (nonce, (byte *) "CurveZMQHELLO---", 16);
    memcpy (nonce + 16, hello->nonce, 8);
    int rc = crypto_box_open (plain, box,
                              crypto_box_BOXZEROBYTES + sizeof (hello->box),
                              nonce, hello->client, self->secret_key);
    assert (rc == 0);       //  In practice, if box doesn't open, discard it
    memcpy (self->cn_client, hello->client, 32);
#endif
}

static zmq_msg_t *
s_produce_welcome (zcurve_t *self)
{
    zmq_msg_close (&self->msg);
    zmq_msg_init_size (&self->msg, sizeof (welcome_t));
    welcome_t *welcome = (welcome_t *) zmq_msg_data (&self->msg);
    memcpy (welcome->id, "WELCOME ", 8);

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
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

    //  Encrypt using symmetric cookie key
    byte cookie_box [96];
    //  Generate fresh cookie key
    randombytes (self->cookie_key, 32);
    rc = crypto_secretbox (cookie_box, plain, 96, nonce, self->cookie_key);
    assert (rc == 0);

    //  Now create 144-byte Box [S' + cookie] (S->C')
    memset (plain, 0, crypto_box_ZEROBYTES);
    memcpy (plain + crypto_box_ZEROBYTES, self->cn_public, 32);
    memcpy (plain + crypto_box_ZEROBYTES + 32, cookie_nonce, 16);
    memcpy (plain + crypto_box_ZEROBYTES + 48,
            cookie_box + crypto_box_BOXZEROBYTES, 80);

    //  Prepare the full nonce and encrypt
    //  8-byte prefix plus 16-byte random nonce
    randombytes (cookie_nonce, 16);
    memcpy (nonce, (byte *) "WELCOME-", 8);
    memcpy (nonce + 8, cookie_nonce, 16);
    rc = crypto_box (box, plain, crypto_box_ZEROBYTES + 128,
                     nonce, self->cn_client, self->secret_key);
    assert (rc == 0);

    memcpy (welcome->nonce, cookie_nonce, 16);
    memcpy (welcome->box,
            box + crypto_box_BOXZEROBYTES, sizeof (welcome->box));
#endif
    return &self->msg;
}

static void
s_process_welcome (zcurve_t *self, welcome_t *welcome)
{
    printf ("S:WELCOME: ");

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
    byte plain [256];

    //  Open Box [S' + cookie](C'->S)
    memset (box, 0, crypto_box_BOXZEROBYTES);
    memcpy (box + crypto_box_BOXZEROBYTES,
            welcome->box, sizeof (welcome->box));

    //  Prepare the full nonce and decrypt
    memcpy (nonce, (byte *) "WELCOME-", 8);
    memcpy (nonce + 8, welcome->nonce, 16);
    int rc = crypto_box_open (plain, box,
                              crypto_box_BOXZEROBYTES + sizeof (welcome->box),
                              nonce, self->server_key, self->cn_secret);
    assert (rc == 0);

    //  Get server cookie and short-term key
    memcpy (self->cn_server, plain + crypto_box_ZEROBYTES, 32);
    memcpy (self->cn_cookie, plain + crypto_box_ZEROBYTES + 32, 96);
    
    //  Pre-compute connection secret from server key
    rc = crypto_box_beforenm (self->cn_precom, self->cn_server, self->cn_secret);
    assert (rc == 0);
#endif
}

static zmq_msg_t *
s_produce_initiate (zcurve_t *self)
{
    zmq_msg_close (&self->msg);
    zmq_msg_init_size (&self->msg, sizeof (initiate_t));
    initiate_t *initiate = (initiate_t *) zmq_msg_data (&self->msg);
    memcpy (initiate->id, "INITIATE", 8);

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
    byte plain [256];

    //  Create vouch = Box [C'](C->S)
    memset (plain, 0, crypto_box_ZEROBYTES);
    memcpy (plain + crypto_box_ZEROBYTES, self->cn_public, 32);
    
    //  Prepare the full nonce and encrypt
    byte vouch_nonce [16];
    randombytes (vouch_nonce, 16);
    memcpy (nonce, (byte *) "VOUCH---", 8);
    memcpy (nonce + 8, vouch_nonce, 16);
    int rc = crypto_box (box, plain, crypto_box_ZEROBYTES + 32,
                     nonce, self->server_key, self->secret_key);
    assert (rc == 0);
    //  Vouch is in box after initial null padding
    byte *vouch_box = box + crypto_box_BOXZEROBYTES;

    //  Create Box [C + nonce + vouch](C'->S')
    memset (plain, 0, crypto_box_ZEROBYTES);
    memcpy (plain + crypto_box_ZEROBYTES, self->public_key, 32);
    memcpy (plain + crypto_box_ZEROBYTES + 32, vouch_nonce, 16);
    memcpy (plain + crypto_box_ZEROBYTES + 48, vouch_box, 48);

    //  Prepare the full nonce and encrypt
    memcpy (nonce, (byte *) "CurveZMQINITIATE", 16);
    memcpy (nonce + 16, &self->cn_nonce, 8);
    rc = crypto_box (box, plain, crypto_box_ZEROBYTES + 96,
                     nonce, self->cn_server, self->cn_secret);
    assert (rc == 0);

    memcpy (initiate->cookie, self->cn_cookie, sizeof (initiate->cookie));
    memcpy (initiate->nonce, &self->cn_nonce, 8);
    memcpy (initiate->box, box + crypto_box_BOXZEROBYTES, sizeof (initiate->box));
    self->cn_nonce++;
#endif
    return &self->msg;
}

static void
s_process_initiate (zcurve_t *self, initiate_t *initiate)
{
    printf ("C:INITIATE: ");

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
    byte plain [256];

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
    //  Destroy cookie key
    memset (self->cookie_key, 0, 32);
    assert (rc == 0);

    //  Check cookie plain text is as expected [C' + s']
    //  In practice we'd reject mismatched cookies, not assert
    assert (memcmp (plain + crypto_box_ZEROBYTES, self->cn_client, 32) == 0);
    assert (memcmp (plain + crypto_box_ZEROBYTES + 32, self->cn_secret, 32) == 0);

    //  Open Box [C + nonce + vouch](C'->S')
    memset (box, 0, crypto_box_BOXZEROBYTES);
    memcpy (box + crypto_box_BOXZEROBYTES, initiate->box, sizeof (initiate->box));

    //  Prepare the full nonce and decrypt
    memcpy (nonce, (byte *) "CurveZMQINITIATE", 16);
    memcpy (nonce + 16, initiate->nonce, 8);
    rc = crypto_box_open (plain, box,
                          crypto_box_BOXZEROBYTES + sizeof (initiate->box),
                          nonce, self->cn_client, self->cn_secret);
    assert (rc == 0);

    //  This is where we'd check the decrypted client public key
    memcpy (self->client_key, plain + crypto_box_ZEROBYTES, 32);

    //  Open vouch Box [C'](C->S) and check contents
    memset (box, 0, crypto_box_BOXZEROBYTES);
    memcpy (box + crypto_box_BOXZEROBYTES, plain + 80, 48);

    //  Prepare the full nonce and decrypt
    memcpy (nonce, (byte *) "VOUCH---", 8);
    memcpy (nonce + 8, plain + 64, 16);
    rc = crypto_box_open (plain, box, crypto_box_BOXZEROBYTES + 48,
                          nonce, self->client_key, self->secret_key);
    assert (rc == 0);

    //  What we decrypted must be the short term client public key
    assert (memcmp (plain + crypto_box_ZEROBYTES, self->cn_client, 32) == 0);

    //  Precompute connection secret from client key
    rc = crypto_box_beforenm (self->cn_precom,
                              self->cn_client, self->cn_secret);
    assert (rc == 0);
#endif
}

static zmq_msg_t *
s_produce_ready (zcurve_t *self)
{
    zmq_msg_close (&self->msg);
    zmq_msg_init_size (&self->msg, sizeof (ready_t));
    ready_t *ready = (ready_t *) zmq_msg_data (&self->msg);
    memcpy (ready->id, "READY   ", 8);
    
#if defined (HAVE_LIBSODIUM)
    memcpy (ready->nonce, &self->cn_nonce, 8);
//     memcpy (ready->box, box + crypto_box_BOXZEROBYTES, sizeof (ready->box));
    self->cn_nonce++;
#endif
    return &self->msg;
}

static void
s_process_ready (zcurve_t *self, ready_t *ready)
{
    printf ("S:READY: ");
    
#if defined (HAVE_LIBSODIUM)
#endif
}

static zmq_msg_t *
s_process_message (zcurve_t *self, message_t *message)
{
    printf ("%c:MESSAGE: ", self->is_server? 'C': 'S');
    
    zmq_msg_close (&self->msg);
    zmq_msg_init_size (&self->msg, 100);

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
    byte plain [256];

    //  Open Box [M](S'->C') or Box [M](C'->S')
    memset (box, 0, crypto_box_BOXZEROBYTES);
    memcpy (box + crypto_box_BOXZEROBYTES, message->box, 100 + crypto_box_BOXZEROBYTES);

    //  Prepare the full nonce and decrypt
    char *prefix = self->is_server? "CurveZMQMESSAGEC": "CurveZMQMESSAGES";
    memcpy (nonce, (byte *) prefix, 16);
    memcpy (nonce + 16, message->nonce, 8);

    int rc = crypto_box_open_afternm (
        plain, box,
        crypto_box_BOXZEROBYTES + sizeof (message->box),
        nonce, self->cn_precom);
    assert (rc == 0);

    memcpy (zmq_msg_data (&self->msg), plain + crypto_box_ZEROBYTES, 100);
#else
    memcpy (zmq_msg_data (&self->msg), message->box, 100);
#endif
    return &self->msg;
}


//  --------------------------------------------------------------------------
//  Accept command from peer. If the command is invalid, it is discarded
//  silently (in this prototype, that causes an assertion failure). May
//  return a zmq_msg_t to send to the peer, or NULL if there is nothing
//  to send.

zmq_msg_t *
zcurve_execute (zcurve_t *self, zmq_msg_t *incoming)
{
    assert (self);

    zmq_msg_t *outgoing = NULL;
    size_t size = incoming? zmq_msg_size (incoming): 0;
    byte *data = (byte *) (incoming? zmq_msg_data (incoming): NULL);

    //  Now process command according to current state
    if (self->state == pending) {
        outgoing = s_produce_hello (self);
        self->state = expect_welcome;
    }
    else
    if (size == sizeof (hello_t)
    &&  memcmp (data, "HELLO   ", 8) == 0
    &&  self->state == expect_hello) {
        s_process_hello (self, (hello_t *) data);
        outgoing = s_produce_welcome (self);
        self->state = expect_initiate;
    }
    else
    if (size == sizeof (welcome_t)
    &&  memcmp (data, "WELCOME ", 8) == 0
    &&  self->state == expect_welcome) {
        s_process_welcome (self, (welcome_t *) data);
        outgoing = s_produce_initiate (self);
        self->state = expect_ready;
    }
    else
    if (size == sizeof (initiate_t)
    &&  memcmp (data, "INITIATE ", 8) == 0
    &&  self->state == expect_initiate) {
        s_process_initiate (self, (initiate_t *) data);
        outgoing = s_produce_ready (self);
        self->state = connected;
    }
    else
    if (size == sizeof (ready_t)
    &&  memcmp (data, "READY   ", 8) == 0
    &&  self->state == expect_ready) {
        s_process_ready (self, (ready_t *) data);
        self->state = connected;
    }
    else
    if (size == sizeof (message_t)
    &&  memcmp (data, "MESSAGE ", 8) == 0
    &&  self->state == connected) {
        outgoing = s_process_message (self, (message_t *) data);
    }
    else {
        puts ("E: invalid command");
        assert (false);
    }
    puts ("OK");
    return outgoing;
}


//  --------------------------------------------------------------------------
//  Encode message from peer. Returns a zmq_msg_t ready to send on the wire.

zmq_msg_t *
zcurve_encode (zcurve_t *self, byte *data, size_t size)
{
    assert (self);
    assert (data);
    assert (self->state == connected);
    
    zmq_msg_close (&self->msg);
    zmq_msg_init_size (&self->msg, sizeof (message_t));
    message_t *message = (message_t *) zmq_msg_data (&self->msg);
    memcpy (message->id, "MESSAGE ", 8);

#if defined (HAVE_LIBSODIUM)
    //  Working variables for crypto calls
    byte nonce [24];
    byte box [256];
    byte plain [256];

    //  Create message Box [M](C'->S'), max M is 100
    memset (plain, 0, crypto_box_ZEROBYTES + 100);
    memcpy (plain + crypto_box_ZEROBYTES, data, size);

    //  Prepare the full nonce and encrypt
    char *prefix = self->is_server? "CurveZMQMESSAGES": "CurveZMQMESSAGEC";
    memcpy (nonce, (byte *) prefix, 16);
    memcpy (nonce + 16, &self->cn_nonce, 8);

    int rc = crypto_box_afternm (
        box, plain,
        crypto_box_ZEROBYTES + 100,
        nonce, self->cn_precom);
    assert (rc == 0);

    memcpy (message->nonce, &self->cn_nonce, 8);
    memcpy (message->box, box + crypto_box_BOXZEROBYTES, sizeof (message->box));
    self->cn_nonce++;
#else
    memset (message->box, 0, sizeof (message->box));
    memcpy (message->box, data, size);
#endif
    return &self->msg;
}


//  --------------------------------------------------------------------------
//  Selftest

//  @selftest
void *
server_task (void *args)
{
    //  We'll use a router socket to be more realistic
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *router = zsocket_new (ctx, ZMQ_ROUTER);
    int rc = zsocket_bind (router, "tcp://*:9000");
    assert (rc != -1);

    //  Create a new server instance and load its keys from
    //  the previously generated keypair file
    zcurve_t *server = zcurve_new (NULL);
    rc = zcurve_keypair_load (server);
    assert (rc == 0);

    //  Hard code the progression for now...
    
    zmq_msg_t identity;
    zmq_msg_t incoming;
    
    //  Get HELLO command
    zmq_msg_init (&identity);
    rc = zmq_msg_recv (&identity, router, 0);
    assert (rc >= 0);
    assert (zmq_msg_more (&identity));
    zmq_msg_init (&incoming);
    rc = zmq_msg_recv (&incoming, router, 0);
    assert (rc >= 0);

    //  Start server side of handshake
    zmq_msg_t *message = zcurve_execute (server, &incoming);
    zmq_msg_close (&incoming);

    //  We have WELCOME, send back to client
    assert (message);
    rc = zmq_msg_send (&identity, router, ZMQ_SNDMORE);
    assert (rc >= 0);
    rc = zmq_msg_send (message, router, 0);
    assert (rc >= 0);

    //  Get INITIATE command
    zmq_msg_init (&identity);
    rc = zmq_msg_recv (&identity, router, 0);
    assert (rc >= 0);
    assert (zmq_msg_more (&identity));
    zmq_msg_init (&incoming);
    rc = zmq_msg_recv (&incoming, router, 0);
    assert (rc >= 0);
    
    message = zcurve_execute (server, &incoming);
    zmq_msg_close (&incoming);
    
    //  We have READY, send back to client
    assert (message);
    rc = zmq_msg_send (&identity, router, ZMQ_SNDMORE);
    assert (rc >= 0);
    rc = zmq_msg_send (message, router, 0);
    assert (rc >= 0);

    //  Get MESSAGE command
    zmq_msg_init (&identity);
    rc = zmq_msg_recv (&identity, router, 0);
    assert (rc >= 0);
    assert (zmq_msg_more (&identity));
    zmq_msg_init (&incoming);
    rc = zmq_msg_recv (&incoming, router, 0);
    assert (rc >= 0);
    message = zcurve_execute (server, &incoming);
    zmq_msg_close (&incoming);

    //  Do Hello, World
    assert (message);
    assert (memcmp (zmq_msg_data (message), "Hello", 5) == 0);
    message = zcurve_encode (server, (byte *) "World", 5);
    assert (message);

    rc = zmq_msg_send (&identity, router, ZMQ_SNDMORE);
    assert (rc >= 0);
    rc = zmq_msg_send (message, router, 0);
    assert (rc >= 0);
    
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
    assert (zfile_size ("secret.key") == 196);
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
    zcurve_keypair_new (client);

    //  Execute null event on client to kick off handshake
    zmq_msg_t *message = zcurve_execute (client, NULL);
    while (message) {
        //  Send message to server
        rc = zmq_msg_send (message, dealer, 0);
        if (rc == -1) puts (strerror (errno));
        assert (rc >= 0);
        //  Get reply from server (could be FSMified)
        rc = zmq_msg_recv (message, dealer, 0);
        assert (rc >= 0);
        //  Execute reply on client to continue handshake
        message = zcurve_execute (client, message);
    }
    //  Handshake is done, now try Hello, World
    message = zcurve_encode (client, (byte *) "Hello", 5);
    assert (message);
    rc = zmq_msg_send (message, dealer, 0);
    assert (rc >= 0);

    rc = zmq_msg_recv (message, dealer, 0);
    assert (rc >= 0);
    message = zcurve_execute (client, message);
    assert (message);
    assert (memcmp (zmq_msg_data (message), "World", 5) == 0);

    //  Done, clean-up
    zcurve_destroy (&client);
    zfile_delete ("public.key");
    zfile_delete ("secret.key");
    //  @end
    
    printf ("OK\n");
}
