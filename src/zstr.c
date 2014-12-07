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

#include "../include/czmq.h"

static int
s_send_string (void *dest, bool more, char *string)
{
    assert (dest);
    void *handle = zsock_resolve (dest);

    int len = strlen (string);
    zmq_msg_t message;
    zmq_msg_init_size (&message, len);
    memcpy (zmq_msg_data (&message), string, len);
    if (zmq_sendmsg (handle, &message, more ? ZMQ_SNDMORE : 0) == -1) {
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
//  Send a C string to a socket, as a frame. The string is sent without
//  trailing null byte; to read this you can use zstr_recv, or a similar
//  method that adds a null terminator on the received string. String
//  may be NULL, which is sent as "".

int
zstr_send (void *dest, const char *string)
{
    assert (dest);
    return s_send_string (dest, false, string ? (char *) string : "");
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
    return s_send_string (dest, true, (char *) string);
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

    int rc = s_send_string (dest, false, string);
    free (string);
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

    int rc = s_send_string (dest, true, string);
    free (string);
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
    return zmsg_send (&msg, dest);
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

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
}
