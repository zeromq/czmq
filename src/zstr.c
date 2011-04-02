/*  =========================================================================
    zstr - sending and receiving strings

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of zapi, the C binding for 0MQ: http://zapi.zeromq.org.

    This is free software; you can redistribute it and/or modify it under the
    terms of the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your option)
    any later version.

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

#include "../include/zapi_prelude.h"
#include "../include/zctx.h"
#include "../include/zstr.h"


//  --------------------------------------------------------------------------
//  Receive C string from socket. Caller must free returned string. Returns
//  NULL if the context is being terminated or the process was interrupted.

char *
zstr_recv (void *socket)
{
    assert (socket);
    zmq_msg_t message;
    zmq_msg_init (&message);
    if (zmq_recv (socket, &message, 0))
        return NULL;

    int size = zmq_msg_size (&message);
    char *string = malloc (size + 1);
    memcpy (string, zmq_msg_data (&message), size);
    zmq_msg_close (&message);
    string [size] = 0;
    return string;
}


//  --------------------------------------------------------------------------
//  Send C string to socket. Returns 0 if successful, -1 if there was an
//  error.

int
zstr_send (void *socket, const char *string)
{
    assert (socket);
    assert (string);

    zmq_msg_t message;
    zmq_msg_init_size (&message, strlen (string));
    memcpy (zmq_msg_data (&message), string, strlen (string));
    int rc = zmq_send (socket, &message, 0);
    zmq_msg_close (&message);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send a string to a socket in 0MQ string format, with MORE flag
int
zstr_sendm (void *socket, const char *string)
{
    assert (socket);
    assert (string);

    zmq_msg_t message;
    zmq_msg_init_size (&message, strlen (string));
    memcpy (zmq_msg_data (&message), string, strlen (string));
    int rc = zmq_send (socket, &message, ZMQ_SNDMORE);
    zmq_msg_close (&message);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send formatted C string to socket

int
zstr_sendf (void *socket, const char *format, ...)
{
    assert (socket);

    //  Format string into buffer
    va_list argptr;
    va_start (argptr, format);
    size_t size = 255 + 1;
    char *string = malloc (size);
    int required = vsnprintf (string, size, format, argptr);
    if (required >= size) {
        size = required + 1;
        string = realloc (string, size);
        vsnprintf (string, size, format, argptr);
    }
    va_end (argptr);

    //  Now send formatted string
    int rc = zstr_send (socket, string);
    free (string);
    return rc;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zstr_test (Bool verbose)
{
    printf (" * zstr: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();

    void *output = zctx_socket_new (ctx, ZMQ_PAIR);
    zmq_bind (output, "inproc://zstr.test");
    void *input = zctx_socket_new (ctx, ZMQ_PAIR);
    zmq_connect (input, "inproc://zstr.test");

    //  Send ten strings and then END
    int string_nbr;
    for (string_nbr = 0; string_nbr < 10; string_nbr++)
        zstr_sendf (output, "this is string %d", string_nbr);
    zstr_send (output, "END");

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
    assert (string_nbr == 10);

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
