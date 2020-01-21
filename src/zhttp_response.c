/*  =========================================================================
    zhttp_response - zhttp server request.
Class can be reused.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zhttp_response - zhttp server request.
Class can be reused.
@discuss
@end
*/

#include "czmq_classes.h"

//  Structure of our class

struct _zhttp_response_t {
    uint32_t status_code;
    zhash_t  *headers;
    char *content;
    bool free_content;
};


//  --------------------------------------------------------------------------
//  Create a new zhttp_response

zhttp_response_t *
zhttp_response_new (void)
{
    zhttp_response_t *self = (zhttp_response_t *) zmalloc (sizeof (zhttp_response_t));
    assert (self);

    self->headers = zhash_new ();
    zhash_autofree (self->headers);
    self->status_code = 200;
    self->content = NULL;
    self->free_content = false;

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zhttp_response

void
zhttp_response_destroy (zhttp_response_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zhttp_response_t *self = *self_p;
        zhash_destroy (&self->headers);

        if (self->free_content)
            zstr_free (&self->content);

        self->content = NULL;
        self->free_content = false;

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

int
zhttp_response_send (zhttp_response_t *self, zsock_t *sock, void **connection_p) {
    assert (self);
    assert (connection_p);
    assert (*connection_p);

    int rc = zsock_bsend (sock, "p4p1p",
            *connection_p, self->status_code, self->headers, self->free_content ? (byte)1 : (byte)0,
            self->content);

    if (rc == -1)
        return rc;

    *connection_p = NULL;
    self->headers = zhash_new ();
    zhash_autofree (self->headers);
    self->content = NULL;
    self->free_content = false;

    return 0;
}


int
zhttp_response_recv (zhttp_response_t *self, zhttp_client_t *client, void** arg_p, void** arg2_p) {
    assert (self);
    assert (client);

    zhash_destroy (&self->headers);
    if (self->free_content) {
        zstr_free (&self->content);
        self->free_content = false;
    }

    uint32_t result;
    int rc = zsock_brecv (client, "4pp4pp", &result, arg_p, arg2_p, &self->status_code, &self->headers, &self->content);

    if (rc == -1) {
        self->headers = zhash_new ();
        zhash_autofree (self->headers);

        return -1;
    }

    self->free_content = self->content != NULL;

    if (result != 0) {
        zhash_destroy(&self->headers);
        self->headers = zhash_new ();
        zhash_autofree (self->headers);

        errno = result;
        return -1;
    }

    return 0;
}

const char *
zhttp_response_content_type (zhttp_response_t *self) {
    assert (self);

    return (const char*)zhash_lookup (self->headers, "Content-Type");
}


void
zhttp_response_set_content_type (zhttp_response_t *self, const char* content_type) {
    assert (self);
    zhash_insert (self->headers, "Content-Type", (void*) content_type);
}


uint32_t
zhttp_response_status_code (zhttp_response_t *self) {
    assert (self);

    return self->status_code;
}


void
zhttp_response_set_status_code (zhttp_response_t *self, uint32_t status_code) {
    assert (self);

    self->status_code = status_code;
}


zhash_t *
zhttp_response_headers (zhttp_response_t *self) {
    assert (self);

    return self->headers;
}


size_t
zhttp_response_content_length (zhttp_response_t *self) {
    assert (self);

    if (!self->content)
        return 0;

    return strlen (self->content);
}

const char *
zhttp_response_content (zhttp_response_t *self) {
    assert (self);

    return self->content;
}


char *
zhttp_response_get_content (zhttp_response_t *self) {
    assert (self);


    if (self->content == NULL)
        return NULL;

    char* content = self->content;

    // If the we don't own the content we can move the ownership, so we must duplicate the content
    if (!self->free_content)
        content = strdup (content);

    self->content = NULL;
    self->free_content = false;

    return content;
}

void
zhttp_response_set_content (zhttp_response_t *self, char **content) {
    assert (self);
    assert (content);

    if (self->free_content)
        zstr_free (&self->content);

    self->free_content = true;
    self->content = *content;
    *content = NULL;
}


void
zhttp_response_set_content_const (zhttp_response_t *self, const char *content) {
    assert (self);
    assert (content);

    if (self->free_content)
        zstr_free (&self->content);

    self->free_content = false;
    self->content = (char*) content;
}


void
zhttp_response_reset_content (zhttp_response_t *self) {
    assert (self);

    if (self->free_content)
        zstr_free (&self->content);

    self->free_content = false;
    self->content = NULL;
}

void
zhttp_response_test (bool verbose) {

}
