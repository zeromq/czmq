/*  =========================================================================
    zhttp_request - zhttp server request.
Class can be reused.

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zhttp_request - zhttp server request.
Class can be reused.
@discuss
@end
*/

#include "czmq_classes.h"

//  Structure of our class

struct _zhttp_request_t {
    char *url;
    char method[256];
    zhash_t *headers;
    char *content;
    bool free_content;
    char *username;
    char *password;
};


//  --------------------------------------------------------------------------
//  Create a new zhttp_request

zhttp_request_t *
zhttp_request_new (void)
{
    zhttp_request_t *self = (zhttp_request_t *) zmalloc (sizeof (zhttp_request_t));
    assert (self);

    self->headers = zhash_new ();
    zhash_autofree (self->headers);
    self->url = strdup ("/");
    strcpy (self->method, "GET");
    self->content = NULL;
    self->free_content = false;
    self->username = NULL;
    self->password = NULL;

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zhttp_request

void
zhttp_request_destroy (zhttp_request_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zhttp_request_t *self = *self_p;

        zstr_free (&self->url);
        zhash_destroy (&self->headers);

        if (self->free_content)
            zstr_free (&self->content);

        self->content = NULL;
        self->free_content = false;

        zstr_free (&self->username);
        zstr_free (&self->password);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


void *
zhttp_request_recv (zhttp_request_t *self, zsock_t *sock) {
    assert (self);

    zstr_free (&self->url);
    zhash_destroy (&self->headers);

    if (self->free_content) {
        zstr_free (&self->content);
        self->free_content = false;
    }

    char *method;
    void* connection;

    int rc = zsock_brecv (sock, "psSpp", &connection, &method, &self->url,
             &self->headers, &self->content);

    if (rc == -1) {
        self->headers = zhash_new ();
        zhash_autofree (self->headers);
        self->url = strdup ("/");

        return NULL;
    }

    self->free_content = self->content != NULL;
    strcpy (self->method, method);

    return connection;
}

int
zhttp_request_send (zhttp_request_t *self, zhttp_client_t *client, int timeout, void *arg, void *arg2) {
    assert (self);
    assert (client);

    int rc = zstr_sendm (client, self->method);
    if (rc == -1)
        return -1;

    zsock_bsend (client, "4ppSp1pss", timeout, arg, arg2, self->url,
            self->headers, self->free_content ? (byte)1 : (byte)0, self->content,
            self->username, self->password);

    self->headers = zhash_new ();
    zhash_autofree (self->headers);
    self->content = NULL;
    self->free_content = false;

    return 0;
}


const char *
zhttp_request_method (zhttp_request_t *self) {
    assert (self);
    return self->method;
}


void
zhttp_request_set_method (zhttp_request_t *self, const char *method) {
    assert (self);
    strcpy (self->method, method);
}


const char *
zhttp_request_url (zhttp_request_t *self) {
    assert (self);
    return self->url;
}


void
zhttp_request_set_url (zhttp_request_t *self, const char *url) {
    assert (self);
    zstr_free (&self->url);
    self->url = strdup (url);
}


const char *
zhttp_request_content_type (zhttp_request_t *self) {
    assert (self);

    return (const char*)zhash_lookup (self->headers, "Content-Type");
}

void
zhttp_request_set_content_type (zhttp_request_t *self, const char *content_type) {
    assert (self);
    zhash_insert (self->headers, "Content-Type", (char*) content_type);
}


size_t
zhttp_request_content_length (zhttp_request_t *self) {
    assert (self);

    if (!self->content)
        return 0;

    return strlen (self->content);
}


zhash_t *
zhttp_request_headers (zhttp_request_t *self) {
    assert (self);

    return self->headers;
}


const char *
zhttp_request_content (zhttp_request_t *self) {
    assert (self);

    return self->content;
}


char *
zhttp_request_get_content (zhttp_request_t *self) {
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
zhttp_request_set_content (zhttp_request_t *self, char **content) {
    assert (self);

    if (self->free_content)
        zstr_free (&self->content);

    self->free_content = true;
    self->content = *content;
    *content = NULL;
}


void
zhttp_request_set_content_const (zhttp_request_t *self, const char *content) {
    assert (self);

    if (self->free_content)
        zstr_free (&self->content);

    self->free_content = false;
    self->content = (char*) content;
}


void
zhttp_request_reset_content (zhttp_request_t *self) {
    assert (self);

    if (self->free_content)
        zstr_free (&self->content);

    self->free_content = false;
    self->content = NULL;
}


void
zhttp_request_set_username (zhttp_request_t *self, const char *username) {
    assert (self);
    zstr_free (&self->username);
    self->username = strdup (username);
}


void
zhttp_request_set_password (zhttp_request_t *self, const char *password) {
    assert (self);
    zstr_free (&self->password);
    self->password = strdup (password);
}


bool
zhttp_request_match (zhttp_request_t *self, const char *method, const char *match, ...) {
    if (strneq (method, self->method))
        return false;

    char* request_needle = self->url;
    const char* match_needle = match;

    // First pass, no allocation, just checking if a match
    while (*request_needle != '\0' && *request_needle != '?' && *match_needle != '\0') {

        // Slashes should always match, no wildcard for them
        if (*request_needle == '/' && *match_needle != '/')
            return false;

        if (*match_needle == '%') {
            match_needle++;

            if (*match_needle == '%') {
                if (*match_needle == *request_needle)
                    return false;

                match_needle++;
                request_needle++;
                continue;
            } else if (*match_needle == 's') {
                match_needle++;

                // Match until / ? or end of string
                while (*request_needle != '/' && *request_needle != '?' && *request_needle != '\0') {
                    request_needle++;
                }

                continue;
            }

            zsys_error ("zhttp_request: invalid path element '%c'");
            assert (false);
        }

        if (*match_needle != *request_needle) {
            return false;
        }

        match_needle++;
        request_needle++;
    }

    // The two strings didn't end at the same offset
    if ((*request_needle != '\0' && *request_needle != '?') || *match_needle != '\0')
        return false;

    va_list argptr;
    va_start (argptr, match);

    match_needle = match;
    request_needle = self->url;

    // another pass, now with allocating the strings, we can skip all the validity checks
    while (*match_needle != '\0') {
        if (*match_needle == '%' && *(match_needle + 1) == '%') {
            request_needle++;
            match_needle += 2;
        }
        else
        if (*match_needle == '%' && *(match_needle + 1) == 's') {
            char* value_start = request_needle;

            while (*request_needle != '/' && *request_needle != '?' && *request_needle != '\0') {
                request_needle++;
            }

            char ** arg = va_arg (argptr, char **);
            if (arg) {
                *arg = value_start;
                *request_needle = '\0';
            }

            match_needle += 2;
        }
        else {
            match_needle++;
            request_needle++;
        }
    }

    va_end (argptr);

    return true;
}


void
zhttp_request_test (bool verbose) {
    printf (" * zhttp_request: ");

    zhttp_request_t *request = zhttp_request_new ();

    zhttp_request_set_method (request, "GET");
    zhttp_request_set_url (request, "/send/hello/world");

    char *s1 = NULL;
    char *s2 = NULL;
    bool is_matched = zhttp_request_match (request, "GET", "/send/%s/%s", &s1, &s2);
    assert (is_matched);

    zhttp_request_set_url (request, "/send/hello/world");
    is_matched = zhttp_request_match (request, "GET", "/send/hello/world");
    assert (is_matched);

    zhttp_request_set_url (request, "/send/hello/world");
    is_matched = zhttp_request_match (request, "POST", "/send/hello/world");
    assert (!is_matched);

    s1 = NULL;
    s2 = NULL;
    zhttp_request_set_url (request, "/send/hello/world");
    is_matched = zhttp_request_match (request, "GET", "/xsend/%s/%s", &s1, &s2);
    assert (!is_matched);
    assert (s1 == NULL);
    assert (s2 == NULL);

    zhttp_request_set_url (request, "/send/hello/world");
    is_matched = zhttp_request_match (request, "GET", "/end/%s/%s", &s1, &s2);
    assert (!is_matched);
    assert (s1 == NULL);
    assert (s2 == NULL);

    zhttp_request_destroy (&request);

    printf ("OK\n");
}
