/*  =========================================================================
    zgossip - gossip discovery service

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
    Implements the ZeroMQ Gossip Discovery Protocol, ZGDP, (RFC TBD).
@discuss
@end
*/

#include "../include/czmq.h"

typedef struct {
    zsock_t *pipe;              //  Socket back to application
} self_t;

static self_t *
    s_constructor (zsock_t *pipe);
static void
    s_destructor (self_t **self_p);
static int
    s_bind (self_t *self);
static int
    s_connect (self_t *self);

//  ---------------------------------------------------------------------
//  

void
zgossip (zsock_t *pipe, void *args)
{
    //  Initialize
    self_t *self = s_constructor (pipe);
    
    //  Signal ready on pipe
    zsock_signal (pipe, 0);

    //  Main actor loop, we must handle $TERM command by terminating
    bool terminated = false;
    while (!terminated) {
        zmsg_t *msg = zmsg_recv (pipe);
        if (!msg)
            break;              //  Interrupted
        char *command = zmsg_popstr (msg);
        if (streq (command, "$TERM"))
            terminated = true;
        else
        if (streq (command, "BIND"))
            zsock_signal (pipe, s_bind (self));
        else
        if (streq (command, "CONNECT"))
            zsock_signal (pipe, s_connect (self));
        else {
            zclock_log ("(zgossip) E: invalid message");
            assert (false);
        }
        free (command);
        zmsg_destroy (&msg);
    }
    s_destructor (&self);
}

static self_t *
s_constructor (zsock_t *pipe)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    assert (self);
    self->pipe = pipe;
    return self;
}

static void
s_destructor (self_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        self_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}

static int
s_bind (self_t *self)
{
    return 0;
}

static int
s_connect (self_t *self)
{
    return 0;
}

//  ---------------------------------------------------------------------
//  Self test of this class

void
zgossip_test (bool verbose)
{
    printf (" * zgossip: ");
    //  @selftest
    zactor_t *base = zactor_new (zgossip, NULL);
    assert (base);
    zactor_t *alpha = zactor_new (zgossip, NULL);
    assert (alpha);
    zactor_t *beta = zactor_new (zgossip, NULL);
    assert (beta);
    
    zstr_sendx (base, "BIND", "inproc://base", NULL);
    zsock_wait (base);
    
    zstr_sendx (alpha, "CONNECT", "inproc://base", NULL);
    zsock_wait (alpha);
    
    zstr_sendx (beta, "CONNECT", "inproc://base", NULL);
    zsock_wait (beta);

    zactor_destroy (&base);
    zactor_destroy (&alpha);
    zactor_destroy (&beta);
    //  @end
    printf ("OK\n");
}
