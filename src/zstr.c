/*  =========================================================================
    zstr - sending and receiving strings

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
    The zstr class provides utility functions for sending and receiving C
    strings across 0MQ sockets. It sends strings without a terminating null,
    and appends a null byte on received strings. This class is for simple
    message sending.
@discuss
@end
*/

#include "../include/czmq.h"

static int
s_send_string (void *zocket, bool more, char *string)
{
    assert (zocket);

    int len = strlen (string);
    zmq_msg_t message;
    zmq_msg_init_size (&message, len);
    memcpy (zmq_msg_data (&message), string, len);
    int rc = zmq_sendmsg (zocket, &message, more? ZMQ_SNDMORE: 0);

    return rc == -1? -1: 0;
}


//  --------------------------------------------------------------------------
//  Receive C string from socket. Caller must free returned string. Returns
//  NULL if the context is being terminated or the process was interrupted.

char *
zstr_recv (void *zocket)
{
    assert (zocket);
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recvmsg (zocket, &message, 0) < 0)
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
//  free returned string. Returns NULL if there was no input waiting, or if
//  the context was terminated. Use zctx_interrupted to exit any loop that
//  relies on this method.

char *
zstr_recv_nowait (void *zocket)
{
    assert (zocket);
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recvmsg (zocket, &message, ZMQ_DONTWAIT) < 0)
        return NULL;

    size_t size = zmq_msg_size (&message);
    char *string = (char *) malloc (size + 1);
    memcpy (string, zmq_msg_data (&message), size);
    zmq_msg_close (&message);
    string [size] = 0;
    return string;
}


//  --------------------------------------------------------------------------
//  Send C string to socket. Returns 0 if successful, -1 if there was an
//  error.

int
zstr_send (void *zocket, const char *format, ...)
{
    assert (zocket);
    assert (format);
    
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    
    int rc = s_send_string (zocket, false, string);
    free (string);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send a string to a socket in 0MQ string format, with MORE flag

int
zstr_sendm (void *zocket, const char *format, ...)
{
    assert (zocket);
    assert (format);
    
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    
    int rc = s_send_string (zocket, true, string);
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
//  Receive a series of strings (until NULL) from multipart data
//  Each string is allocated and filled with string data; if there
//  are not enough frames, unallocated strings are set to NULL.
//  Returns -1 if the message could not be read, else returns the
//  number of strings filled, zero or more.

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
    return 0;
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
        zstr_send (output, "this is string %d", string_nbr);
    zstr_sendx (output, "This", "is", "almost", "the", "very", "END", NULL);

    //  Read and count until we receive END
    string_nbr = 0;
    for (string_nbr = 0;; string_nbr++) {
        char *string = zstr_recv (input);
        if (streq (string, "END")) {
            free (string);
            break;
        }
        free (string);
    }
    assert (string_nbr == 15);

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
