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
@end
*/

#include "../include/czmq.h"

static int
s_send_string (void *socket, bool more, char *string)
{
    assert (socket);

    int len = strlen (string);
    zmq_msg_t message;
    zmq_msg_init_size (&message, len);
    memcpy (zmq_msg_data (&message), string, len);
    if (zmq_sendmsg (socket, &message, more? ZMQ_SNDMORE: 0) == -1) {
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
zstr_recv (void *socket)
{
    assert (socket);
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recvmsg (socket, &message, 0) < 0)
        return NULL;

    size_t size = zmq_msg_size (&message);
    char *string = (char *) malloc (size + 1);
    memcpy (string, zmq_msg_data (&message), size);
    zmq_msg_close (&message);
    string [size] = 0;
    return string;
}


//  --------------------------------------------------------------------------
//  Receive C string from socket, if socket had input ready. Caller must
//  free returned string using zstr_free. Returns NULL if there was no input
//  waiting, or if the context was terminated. Use zctx_interrupted to exit
//  any loop that relies on this method.

char *
zstr_recv_nowait (void *socket)
{
    assert (socket);
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recvmsg (socket, &message, ZMQ_DONTWAIT) < 0)
        return NULL;

    size_t size = zmq_msg_size (&message);
    char *string = (char *) malloc (size + 1);
    memcpy (string, zmq_msg_data (&message), size);
    zmq_msg_close (&message);
    string [size] = 0;
    return string;
}


//  --------------------------------------------------------------------------
//  Send a C string to a socket, as a frame. The string is sent without
//  trailing null byte; to read this you can use zstr_recv, or a similar
//  method that adds a null terminator on the received string.

int
zstr_send (void *socket, const char *string)
{
    assert (socket);
    assert (string);
    return s_send_string (socket, false, (char *) string);
}


//  --------------------------------------------------------------------------
//  Send a C string to a socket, as zstr_send(), with a MORE flag, so that
//  you can send further strings in the same multi-part message.

int
zstr_sendm (void *socket, const char *string)
{
    assert (socket);
    assert (string);
    return s_send_string (socket, true, (char *) string);
}


//  --------------------------------------------------------------------------
//  Send a formatted string to a socket. Note that you should NOT use
//  user-supplied strings in the format (they may contain '%' which
//  will create security holes).

int
zstr_sendf (void *socket, const char *format, ...)
{
    assert (socket);
    assert (format);

    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);

    int rc = s_send_string (socket, false, string);
    free (string);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send a formatted string to a socket, as for zstr_sendf(), with a
//  MORE flag, so that you can send further strings in the same multi-part
//  message.

int
zstr_sendfm (void *socket, const char *format, ...)
{
    assert (socket);
    assert (format);

    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);

    int rc = s_send_string (socket, true, string);
    free (string);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send a series of strings (until NULL) as multipart data
//  Returns 0 if the strings could be sent OK, or -1 on error.

int
zstr_sendx (void *socket, const char *string, ...)
{
    zmsg_t *msg = zmsg_new ();
    va_list args;
    va_start (args, string);
    while (string) {
        zmsg_addstr (msg, string);
        string = va_arg (args, char *);
    }
    va_end (args);
    return zmsg_send (&msg, socket);
}


//  --------------------------------------------------------------------------
//  Receive a series of strings (until NULL) from multipart data.
//  Each string is allocated and filled with string data; if there
//  are not enough frames, unallocated strings are set to NULL.
//  Returns -1 if the message could not be read, else returns the
//  number of strings filled, zero or more. Free each returned string
//  using zstr_free().

int
zstr_recvx (void *socket, char **string_p, ...)
{
    zmsg_t *msg = zmsg_recv (socket);
    if (!msg)
        return -1;
        
    va_list args;
    va_start (args, string_p);
    while (string_p) {
        *string_p = zmsg_popstr (msg);
        string_p = va_arg (args, char **);
    }
    va_end (args);
    zmsg_destroy (&msg);
    return 0;
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
//  Selftest

int
zstr_test (bool verbose)
{
    printf (" * zstr: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);

    void *output = zsocket_new (ctx, ZMQ_PAIR);
    assert (output);
    zsocket_bind (output, "inproc://zstr.test");
    void *input = zsocket_new (ctx, ZMQ_PAIR);
    assert (input);
    zsocket_connect (input, "inproc://zstr.test");

    //  Send ten strings, five strings with MORE flag and then END
    int string_nbr;
    for (string_nbr = 0; string_nbr < 10; string_nbr++)
        zstr_sendf (output, "this is string %d", string_nbr);
    zstr_sendx (output, "This", "is", "almost", "the", "very", "END", NULL);

    //  Read and count until we receive END
    string_nbr = 0;
    for (string_nbr = 0;; string_nbr++) {
        char *string = zstr_recv (input);
        if (streq (string, "END")) {
            zstr_free (&string);
            break;
        }
        zstr_free (&string);
    }
    assert (string_nbr == 15);

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
