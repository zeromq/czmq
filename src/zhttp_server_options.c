/*  =========================================================================
    zhttp_server_options - class description

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zhttp_server_options -
@discuss
@end
*/

#include "czmq_classes.h"

//  Structure of our class

struct _zhttp_server_options_t {
    int port;
    char *backend_address;
};


//  --------------------------------------------------------------------------
//  Create a new zhttp_server_options

zhttp_server_options_t *
zhttp_server_options_new (void)
{
    zhttp_server_options_t *self = (zhttp_server_options_t *) zmalloc (sizeof (zhttp_server_options_t));

    self->port = 8080;
    self->backend_address =
            zsys_sprintf ("inproc://zhttp_server-%04x-%04x", randof (0x10000), randof (0x10000));

    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zhttp_server_options

void
zhttp_server_options_destroy (zhttp_server_options_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zhttp_server_options_t *self = *self_p;
        zstr_free (&self->backend_address);
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


zhttp_server_options_t *
zhttp_server_options_from_config (zconfig_t *config) {
    assert (config);
    zhttp_server_options_t *self = zhttp_server_options_new ();

    const char* backend_address = zconfig_get (config, "http_server/backend_address", NULL);
    const char* port_str = zconfig_get (config, "http_server/port", NULL);

    if (backend_address)
        self->backend_address = strdup (backend_address);

    if (port_str)
        self->port = atoi (port_str);

    return self;
}

int
zhttp_server_options_port (zhttp_server_options_t *self) {
    assert (self);

    return self->port;
}

void
zhttp_server_options_set_port (zhttp_server_options_t *self, int port) {
    self->port = port;
}

const char *
zhttp_server_options_backend_address (zhttp_server_options_t *self) {
    assert (self);

    return self->backend_address;
}

void
zhttp_server_options_set_backend_address (zhttp_server_options_t *self, const char *address) {
    assert (self);
    self->backend_address = strdup (address);
}

void
zhttp_server_options_test (bool verbose) {

}