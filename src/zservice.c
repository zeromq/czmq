/*  =========================================================================
    zservice - network service registry and lookup

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
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
    The zservice class does network service registry and lookup. A zservice
    instance can register zero or more services, and perform zero or more
    service lookups. A service is defined as a name, and a ZeroMQ endpoint.
@discuss
    Current implementation uses UDP broadcasts and unicasts.
    - ZRE port 5670
    - or request-reply
    - service request:
        - unicast reply with endpoint
@end
*/

#include "../include/czmq.h"

//  Structure of our class
struct _zservice_t {
    void *pipe;                 //  Pipe through to backend agent
};


//  Background service agent does the real work
static void
    s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Create a new service instance

zservice_t *
zservice_new (zctx_t *ctx)
{
    zservice_t *self = (zservice_t *) zmalloc (sizeof (zservice_t));
    assert (self);

    //  Start background agent to connect to the service agent
    assert (ctx);
    self->pipe = zthread_fork (ctx, s_agent_task, NULL);
    if (self->pipe) {
        char *status = zstr_recv (self->pipe);
        if (strneq (status, "OK"))
            zservice_destroy (&self);
        zstr_free (&status);
    }
    else {
        free (self);
        self = NULL;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a service instance

void
zservice_destroy (zservice_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zservice_t *self = *self_p;
        zstr_send (self->pipe, "TERMINATE");
        char *reply = zstr_recv (self->pipe);
        zstr_free (&reply);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Enable verbose tracing of commands and activity

void
zservice_set_verbose (zservice_t *self, bool verbose)
{
    assert (self);
    zstr_sendm (self->pipe, "VERBOSE");
    zstr_sendf (self->pipe, "%d", verbose);
}


//  --------------------------------------------------------------------------
//  Register a new network service

void
zservice_register (zservice_t *self, const char *name, const char *endpoint)
{
}


//  --------------------------------------------------------------------------
//  Lookup a network service, return endpoint or NULL if not known

char *
zservice_lookup (zservice_t *self, const char *name)
{
    return NULL;
}


//  --------------------------------------------------------------------------
//  Backend agent implementation

//  Agent instance

typedef struct {
    zctx_t *ctx;
    void *pipe;             //  Socket back to application
    bool verbose;           //  Trace activity to stdout
    bool terminated;
} agent_t;

//  Prototypes for local functions we use in the agent
static agent_t *
    s_agent_new (zctx_t *ctx, void *pipe);
static void
    s_api_command (agent_t *self);
static void
    s_agent_destroy (agent_t **self_p);


//  This is the background task that monitors socket events

static void
s_agent_task (void *args, zctx_t *ctx, void *pipe)
{
    agent_t *self = s_agent_new (ctx, pipe);
    zpoller_t *poller = zpoller_new (self->pipe, NULL);
    while (!zctx_interrupted) {
        //  Poll on API pipe and on monitor socket
        void *result = zpoller_wait (poller, -1);
        if (result == NULL)
            break; // Interrupted
        else
        if (result == self->pipe)
            s_api_command (self);

        if (self->terminated)
            break;
    }
    zpoller_destroy (&poller);
    s_agent_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Create and initialize new agent instance

static agent_t *
s_agent_new (zctx_t *ctx, void *pipe)
{
    agent_t *self = (agent_t *) malloc (sizeof (agent_t));
    assert (self);
    self->ctx = ctx;
    self->pipe = pipe;
    zstr_send (self->pipe, "OK");
    return self;
}


//  --------------------------------------------------------------------------
//  Handle command from API

static void
s_api_command (agent_t *self)
{
    char *command = zstr_recv (self->pipe);
    if (streq (command, "TERMINATE")) {
        self->terminated = true;
        zstr_send (self->pipe, "OK");
    }
    else
    if (streq (command, "VERBOSE")) {
        char *verbose = zstr_recv (self->pipe);
        self->verbose = *verbose == '1';
        free (verbose);
    }
    else
        printf ("E: zservice unexpected API command '%s'\n", command);

    free (command);
}


//  --------------------------------------------------------------------------
//  Destroy agent instance

static void
s_agent_destroy (agent_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        agent_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Selftest of this class

void
zservice_test (bool verbose)
{
    printf (" * zservice: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    zservice_t *service = zservice_new (ctx);
    zservice_destroy (&service);
    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
}
