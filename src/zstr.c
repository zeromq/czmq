/*  =========================================================================
    zstr - sending and receiving strings

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The zstr class provides utility functions for sending and receiving C
    strings across 0MQ sockets. It sends strings without a terminating null,
    and appends a null byte on received strings. This class is for simple
    message sending.
@discuss
           Memory                       Wire
           +-------------+---+          +---+-------------+
    Send   | S t r i n g | 0 |  ---->   | 6 | S t r i n g |
           +-------------+---+          +---+-------------+

           Wire                         Heap
           +---+-------------+          +-------------+---+
    Recv   | 6 | S t r i n g |  ---->   | S t r i n g | 0 |
           +---+-------------+          +-------------+---+
@end
*/

#include "czmq_classes.h"
#ifdef HAVE_LIBLZ4
#include <lz4.h>

static void
s_free_compress (void *data, void *hint)
{
    free (data);
}
#endif

static int
s_send_string (void *dest, bool more, char *string, bool compress)
{
    assert (dest);
    void *handle = zsock_resolve (dest);

    size_t len = strlen (string);
    zmq_msg_t message;

    if (compress) {
#ifdef HAVE_LIBLZ4
        size_t compressed_len = LZ4_compressBound (len);
        if (compressed_len == 0)
            return -1;

        char *buffer = (char *)malloc (compressed_len );
        if (!buffer)
            return -1;

        //  LZ4_compress has been deprecated in newer versions, but
        //  the new APIs are not available for older distros
#if LZ4_VERSION_MAJOR >= 1 && LZ4_VERSION_MINOR >= 7
        int rc = LZ4_compress_default (string, buffer, len, compressed_len);
#else
        int rc = LZ4_compress (string, buffer, len);
#endif
        if (rc == 0) {
            free (buffer);
            return -1;
        }

        zmq_msg_t size_frame;
        zmq_msg_init_size (&size_frame, sizeof (size_t));
        memcpy (zmq_msg_data (&size_frame), &len, sizeof (size_t));

#if defined (ZMQ_SERVER)
        //  Set routing ID if we're sending to a SERVER socket (ZMQ 4.2 and later)
        if (zsock_is (dest) && zsock_type (dest) == ZMQ_SERVER)
            zmq_msg_set_routing_id (&size_frame, zsock_routing_id ((zsock_t *) dest));
#endif
        if (zmq_sendmsg (handle, &size_frame, ZMQ_SNDMORE) == -1) {
            free (buffer);
            zmq_msg_close (&size_frame);
            return -1;
        }

        //  Optimisation: use zero-copy send. The common use case for
        //  compressed data is large buffers, so avoiding an extra malloc + copy
        //  is worth the extra few lines of code
        rc = zmq_msg_init_data (&message, buffer, rc, s_free_compress, NULL);
        //  Assert on OOM
        assert (rc != -1);
#else
        return -1;
#endif
    } else {
        int rc = zmq_msg_init_size (&message, len);
        assert (rc == 0);
        memcpy (zmq_msg_data (&message), string, len);
    }

#if defined (ZMQ_SERVER)
    //  Set routing ID if we're sending to a SERVER socket (ZMQ 4.2 and later)
    if (zsock_is (dest) && zsock_type (dest) == ZMQ_SERVER)
        zmq_msg_set_routing_id (&message, zsock_routing_id ((zsock_t *) dest));
#endif
    if (zmq_sendmsg (handle, &message, more? ZMQ_SNDMORE: 0) == -1) {
        zmq_msg_close (&message);
        return -1;
    }
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Receive C string from socket. Caller must free returned string using
//  zstr_free(). Returns NULL if the context is being terminated or the
//  process was interrupted.

char *
zstr_recv (void *source)
{
    assert (source);
    void *handle = zsock_resolve (source);

    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recvmsg (handle, &message, 0) < 0)
        return NULL;

#if defined (ZMQ_SERVER)
    //  Grab routing ID if we're reading from a SERVER socket (ZMQ 4.2 and later)
    if (zsock_is (source) && zsock_type (source) == ZMQ_SERVER)
        zsock_set_routing_id ((zsock_t *) source, zmq_msg_routing_id (&message));
#endif
    size_t size = zmq_msg_size (&message);
    char *string = (char *) malloc (size + 1);
    if (string) {
        memcpy (string, zmq_msg_data (&message), size);
        string [size] = 0;
    }
    zmq_msg_close (&message);
    return string;
}


//  --------------------------------------------------------------------------
//  Receive a series of strings (until NULL) from multipart data.
//  Each string is allocated and filled with string data; if there
//  are not enough frames, unallocated strings are set to NULL.
//  Returns -1 if the message could not be read, else returns the
//  number of strings filled, zero or more. Free each returned string
//  using zstr_free(). If not enough strings are provided, remaining
//  multipart frames in the message are dropped.

int
zstr_recvx (void *source, char **string_p, ...)
{
    assert (source);
    void *handle = zsock_resolve (source);

    zmsg_t *msg = zmsg_recv (handle);
    if (!msg)
        return -1;

#if defined (ZMQ_SERVER)
    //  Grab routing ID if we're reading from a SERVER socket (ZMQ 4.2 and later)
    if (zsock_is (source) && zsock_type (source) == ZMQ_SERVER)
        zsock_set_routing_id ((zsock_t *) source, zmsg_routing_id (msg));
#endif
    //  Filter a signal that may come from a dying actor
    if (zmsg_signal (msg) >= 0) {
        zmsg_destroy (&msg);
        return -1;
    }
    int count = 0;
    va_list args;
    va_start (args, string_p);
    while (string_p) {
        *string_p = zmsg_popstr (msg);
        string_p = va_arg (args, char **);
        count++;
    }
    va_end (args);
    zmsg_destroy (&msg);
    return count;
}


//  --------------------------------------------------------------------------
//  De-compress and receive C string from socket, received as a message
//  with two frames: size of the uncompressed string, and the string itself.
//  Caller must free returned string using zstr_free(). Returns NULL if the
//  context is being terminated or the process was interrupted.
//  Caller owns return value and must destroy it when done.

char *
zstr_recv_compress (void *source)
{
    assert (source);
#ifndef HAVE_LIBLZ4
    return NULL;
#else

    void *handle = zsock_resolve (source);

    zmsg_t *msg = zmsg_recv (handle);
    if (!msg)
        return NULL;

#if defined (ZMQ_SERVER)
    //  Grab routing ID if we're reading from a SERVER socket (ZMQ 4.2 and later)
    if (zsock_is (source) && zsock_type (source) == ZMQ_SERVER)
        zsock_set_routing_id ((zsock_t *) source, zmsg_routing_id (msg));
#endif
    //  Filter a signal that may come from a dying actor
    if (zmsg_signal (msg) >= 0) {
        zmsg_destroy (&msg);
        return NULL;
    }

    //  Size and data
    if (zmsg_size (msg) != 2) {
        zmsg_destroy (&msg);
        return NULL;
    }

    size_t size = *((size_t *)zframe_data (zmsg_first (msg)));
    char *string = (char *) malloc (size + 1);
    if (string) {
        zframe_t *data_frame = zmsg_next (msg);
        int rc = LZ4_decompress_safe ((char *)zframe_data (data_frame),
            string, zframe_size (data_frame), size);
        string [size] = 0;
        if (rc < 0) {
            zstr_free (&string);
        }
    }
    zmsg_destroy (&msg);
    return string;
#endif
}


//  --------------------------------------------------------------------------
//  Send a C string to a socket, as a frame. The string is sent without
//  trailing null byte; to read this you can use zstr_recv, or a similar
//  method that adds a null terminator on the received string. String
//  may be NULL, which is sent as "".

int
zstr_send (void *dest, const char *string)
{
    assert (dest);
    return s_send_string (dest, false, string? (char *) string: "", false);
}


//  --------------------------------------------------------------------------
//  Send a C string to a socket, as zstr_send(), with a MORE flag, so that
//  you can send further strings in the same multi-part message. String
//  may be NULL, which is sent as "".

int
zstr_sendm (void *dest, const char *string)
{
    assert (dest);
    assert (string);
    return s_send_string (dest, true, (char *) string, false);
}


//  --------------------------------------------------------------------------
//  Send a formatted string to a socket. Note that you should NOT use
//  user-supplied strings in the format (they may contain '%' which
//  will create security holes).

int
zstr_sendf (void *dest, const char *format, ...)
{
    assert (dest);
    assert (format);

    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    if (!string)
        return -1;

    va_end (argptr);

    int rc = s_send_string (dest, false, string, false);
    zstr_free (&string);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send a formatted string to a socket, as for zstr_sendf(), with a
//  MORE flag, so that you can send further strings in the same multi-part
//  message.

int
zstr_sendfm (void *dest, const char *format, ...)
{
    assert (dest);
    assert (format);

    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    if (!string)
        return -1;

    va_end (argptr);

    int rc = s_send_string (dest, true, string, false);
    zstr_free (&string);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send a series of strings (until NULL) as multipart data
//  Returns 0 if the strings could be sent OK, or -1 on error.

int
zstr_sendx (void *dest, const char *string, ...)
{
    zmsg_t *msg = zmsg_new ();
    if (!msg)
        return -1;
    va_list args;
    va_start (args, string);
    while (string) {
        zmsg_addstr (msg, string);
        string = va_arg (args, char *);
    }
    va_end (args);

#if defined (ZMQ_SERVER)
    //  Grab routing ID if we're reading from a SERVER socket (ZMQ 4.2 and later)
    if (zsock_is (dest) && zsock_type (dest) == ZMQ_SERVER)
        zmsg_set_routing_id (msg, zsock_routing_id ((zsock_t *) dest));
#endif
    if (zmsg_send (&msg, dest) < 0) {
        zmsg_destroy(&msg);
        return -1;
    }
    else
        return 0;
}

//  Compress and send a C string to a socket, as a message with two frames:
//  size of the uncompressed string, and the string itself. The string is
//  sent without trailing null byte; to read this you can use
//  zstr_recv_compress, or a similar method that de-compresses and adds a
//  null terminator on the received string.
int
zstr_send_compress (void *dest, const char *string)
{
    assert (dest);
    return s_send_string (dest, false, (char *) string, true);
}

//  Compress and send a C string to a socket, as zstr_send_compress(),
//  with a MORE flag, so that you can send further strings in the same
//  multi-part message.
int
zstr_sendm_compress (void *dest, const char *string)
{
    assert (dest);
    assert (string);
    return s_send_string (dest, true, (char *) string, true);
}

//  --------------------------------------------------------------------------
//  Accepts a void pointer and returns a fresh character string. If source is
//  null, returns an empty string.

char *
zstr_str (void *source)
{
    if (source)
        return strdup ((char *) source);
    else
        return strdup ("");
}


//  --------------------------------------------------------------------------
//  Free a provided string, and nullify the parent pointer. Safe to call on
//  a null pointer.

void
zstr_free (char **string_p)
{
    assert (string_p);
    free (*string_p);
    *string_p = NULL;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as poor style -- callers should use zloop or zpoller
//  Receive C string from socket, if socket had input ready. Caller must
//  free returned string using zstr_free. Returns NULL if there was no input
//  waiting, or if the context was terminated. Use zsys_interrupted to exit
//  any loop that relies on this method.

char *
zstr_recv_nowait (void *dest)
{
    assert (dest);
    void *handle = zsock_resolve (dest);

    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recvmsg (handle, &message, ZMQ_DONTWAIT) < 0)
        return NULL;

    size_t size = zmq_msg_size (&message);
    char *string = (char *) malloc (size + 1);
    if (string) {
        memcpy (string, zmq_msg_data (&message), size);
        string [size] = 0;
    }
    zmq_msg_close (&message);
    return string;
}


//  --------------------------------------------------------------------------
//  Selftest

void
zstr_test (bool verbose)
{
    printf (" * zstr: ");

    //  @selftest
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new_pair ("@inproc://zstr.test");
    assert (output);
    zsock_t *input = zsock_new_pair (">inproc://zstr.test");
    assert (input);

    //  Send ten strings, five strings with MORE flag and then END
    int string_nbr;
    for (string_nbr = 0; string_nbr < 10; string_nbr++)
        zstr_sendf (output, "this is string %d", string_nbr);
    zstr_sendx (output, "This", "is", "almost", "the", "very", "END", NULL);

    //  Read and count until we receive END
    string_nbr = 0;
    for (string_nbr = 0;; string_nbr++) {
        char *string = zstr_recv (input);
        assert (string);
        if (streq (string, "END")) {
            zstr_free (&string);
            break;
        }
        zstr_free (&string);
    }
    assert (string_nbr == 15);

#ifdef HAVE_LIBLZ4
    int ret = zstr_send_compress (output, "loooong");
    assert (ret == 0);
    char *string = zstr_recv_compress (input);
    assert (string);
    assert (streq (string, "loooong"));
    zstr_free (&string);

    zstr_send_compress (output, "loooong");
    assert (ret == 0);
    zmsg_t *msg = zmsg_recv (input);
    assert (msg);
    assert (*((size_t *)zframe_data (zmsg_first (msg))) == strlen ("loooong"));
    zmsg_destroy (&msg);
#endif

    zsock_destroy (&input);
    zsock_destroy (&output);

#if defined (ZMQ_SERVER)
    //  Test SERVER/CLIENT over zstr
    zsock_t *server = zsock_new_server ("inproc://zstr-test-routing");
    zsock_t *client = zsock_new_client ("inproc://zstr-test-routing");;
    assert (server);
    assert (client);

    //  Try normal ping-pong to check reply routing ID
    int rc = zstr_send (client, "Hello");
    assert (rc == 0);
    char *request = zstr_recv (server);
    assert (streq (request, "Hello"));
    assert (zsock_routing_id (server));
    freen (request);

    rc = zstr_send (server, "World");
    assert (rc == 0);
    char *reply = zstr_recv (client);
    assert (streq (reply, "World"));
    freen (reply);

    rc = zstr_sendf (server, "%s", "World");
    assert (rc == 0);
    reply = zstr_recv (client);
    assert (streq (reply, "World"));
    freen (reply);

    //  Try ping-pong using sendx and recx
    rc = zstr_sendx (client, "Hello", NULL);
    assert (rc == 0);
    rc = zstr_recvx (server, &request, NULL);
    assert (rc >= 0);
    assert (streq (request, "Hello"));
    freen (request);

    rc = zstr_sendx (server, "World", NULL);
    assert (rc == 0);
    rc = zstr_recvx (client, &reply, NULL);
    assert (rc >= 0);
    assert (streq (reply, "World"));
    freen (reply);

    //  Client and server disallow multipart
    rc = zstr_sendm (client, "Hello");
    assert (rc == -1);
    rc = zstr_sendm (server, "World");
    assert (rc == -1);

    zsock_destroy (&client);
    zsock_destroy (&server);
#endif

#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end

    printf ("OK\n");
}
