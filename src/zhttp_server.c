/*  =========================================================================
    zhttp_server -

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zhttp_server -
@discuss
@end
*/

#include "czmq_classes.h"

#ifdef HAVE_LIBMICROHTTPD
#include <microhttpd.h>
#endif

#ifdef HAVE_LIBMICROHTTPD

typedef struct {
    char* content;
    size_t content_length;
    zhash_t *headers;
    bool suspended;
} request_t;

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?

    zhttp_server_options_t *options;
    zsock_t *backend;
    struct MHD_Daemon *daemon;
    zframe_t *empty_frame;
    zlistx_t* pending_connections;
} server_actor_t;

static int
s_handle_request (
        server_actor_t *self,
        struct MHD_Connection *mhd_connection,
        const char *url,
        const char *method,
        const char *version,
        const char *upload_data,
        size_t *upload_data_size,
        request_t **request_p);

static void
s_handle_request_completed (
        server_actor_t *self,
        struct MHD_Connection *mhd_connection,
        request_t **request_p,
        enum MHD_RequestTerminationCode toe);

static void
server_actor (zsock_t *pipe, void *args);


static request_t*
request_new () {
    request_t *self = (request_t*)zmalloc (sizeof (request_t));
    assert (self);

    self->headers = zhash_new ();
    zhash_autofree (self->headers);

    return self;
}

static void
request_destroy (request_t **self_p) {
    request_t * self= *self_p;

    if (self) {
        zhash_destroy (&self->headers);
        zstr_free (&self->content);

        free (self);
    }

    *self_p = NULL;
}

static void *
s_insert_connection (struct MHD_Connection *connection) {
#if MHD_VERSION >= 0x00093400
    MHD_suspend_connection (connection);
#endif
    return connection;
}

static void
s_destroy_connection (struct MHD_Connection **connection) {
#if MHD_VERSION >= 0x00093400
    MHD_resume_connection (*connection);
#endif
    *connection = NULL;
}

static server_actor_t *
server_actor_new (zsock_t *pipe, zhttp_server_options_t *options)
{
    server_actor_t *self = (server_actor_t *) zmalloc (sizeof (server_actor_t));
    assert (self);

    self->daemon = MHD_start_daemon (
#if MHD_VERSION >= 0x00095300
            MHD_ALLOW_SUSPEND_RESUME,
#elif MHD_VERSION >= 0x00093400
            MHD_USE_SUSPEND_RESUME,
#else
            MHD_USE_PIPE_FOR_SHUTDOWN,
#endif
            (uint16_t ) zhttp_server_options_port (options),
            NULL, NULL,
            (MHD_AccessHandlerCallback) s_handle_request, self,
            MHD_OPTION_NOTIFY_COMPLETED, s_handle_request_completed, self,
            MHD_OPTION_END);

    if (self->daemon == NULL) {
        freen (self);
        return NULL;
    }

    self->pipe = pipe;
    self->terminated = false;
    self->options = options;
    self->empty_frame = zframe_new_empty ();
    self->pending_connections = zlistx_new ();
    zlistx_set_destructor (self->pending_connections, (zlistx_destructor_fn*) s_destroy_connection);
    zlistx_set_duplicator (self->pending_connections, (zlistx_duplicator_fn*) s_insert_connection);
    self->backend = zsock_new_dealer (NULL);
    zsock_bind (self->backend, "%s", zhttp_server_options_backend_address (options));

    return self;
}

static void
server_actor_destroy (server_actor_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        server_actor_t *self = *self_p;
        zframe_destroy (&self->empty_frame);
        zsock_destroy (&self->backend);

        zlistx_destroy (&self->pending_connections);
        MHD_stop_daemon (self->daemon);
        self->daemon = NULL;

        free (self);
        *self_p = NULL;
    }
}


static void
s_recv_api (server_actor_t *self) {
    //  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "VERBOSE"))
        self->verbose = true;
    else
    if (streq (command, "PORT")) {
#if MHD_VERSION >= 0x00095600
        const union MHD_DaemonInfo *info = MHD_get_daemon_info (self->daemon, MHD_DAEMON_INFO_BIND_PORT);
        assert (info);
        zsock_send (self->pipe, "i", (int)info->port);
#else
        int port = zhttp_server_options_port (self->options);

        if (port == 0)
            zsock_send (self->pipe, "i", -1);
        else
            zsock_send (self->pipe, "i", port);
#endif
    }
    else
    if (streq (command, "$TERM"))
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
}


static int
s_request_add_header (zhash_t *headers, enum MHD_ValueKind kind, const char *key, const char *value) {
    if (!key || !value)
        return MHD_YES;     //  Ignore connection value if invalid!

    zhash_insert (headers, (char *) key, (char *) value);

    return MHD_YES;
}

static int
s_handle_request (
        server_actor_t *self,
        struct MHD_Connection *connection,
        const char *path,
        const char *method,
        const char *version,
        const char *upload_data,
        size_t *upload_data_size,
        request_t **request_p) {
    assert (self);

    request_t *request = *request_p;

    if (request && request->suspended)
        return MHD_YES;

    if (request == NULL) {
        request = request_new ();
        *request_p = request;

        // Get the headers
        MHD_get_connection_values (connection,
                MHD_HEADER_KIND, (MHD_KeyValueIterator) s_request_add_header, request->headers);

        // Request with post data, next calls will provide the data
        if (zhash_lookup (request->headers, "Content-Length") ||
            zhash_lookup (request->headers, "Transfer-Encoding")) {

            // TODO: check if POST_DATA is on MHD_POSTDATA_KIND

            return MHD_YES;
        }

        // Suspending the connection until response is ready
        zlistx_add_end (self->pending_connections, connection);
        request->suspended = true;

        zsock_bsend (self->backend, "psSpp", connection, method, path, request->headers, NULL);
        request->headers = NULL;

        return MHD_YES;
    }

    // Append additional data
    if (*upload_data_size != 0) {
        size_t offset = 0;

        if (request->content == NULL) {
            request->content_length = *upload_data_size;
            request->content = (char*) malloc (request->content_length + 1);
        } else {
            offset = request->content_length;
            request->content_length += *upload_data_size;
            request->content = (char *) realloc (request->content, request->content_length + 1);
        }

        memcpy (request->content + offset, upload_data, *upload_data_size);
        request->content[request->content_length] = '\0';
        *upload_data_size = 0;

        return MHD_YES;
    }

    // Suspending the connection until response is ready
    zlistx_add_end (self->pending_connections, connection);
    request->suspended = true;

    zsock_bsend (self->backend, "psSpp",
            connection, method, path, request->headers, request->content);
    request->content = NULL;
    request->headers = NULL;

    return MHD_YES;
}

static void
s_handle_request_completed (
        server_actor_t *self,
        struct MHD_Connection *connection,
        request_t **request_p,
        enum MHD_RequestTerminationCode toe) {
    request_destroy (request_p);
}

static void
s_recv_backend (server_actor_t *self) {
    zhash_t *headers;
    char *content;
    struct MHD_Connection *connection;
    uint32_t status_code;
    byte free_content;

    int rc = zsock_brecv (self->backend, "p4p1p", &connection, &status_code, &headers, &free_content, &content);
    if (rc == -1)
        return;

    struct MHD_Response *http_response;

    if (content) {
        http_response = MHD_create_response_from_buffer (
                strlen (content), content,
                free_content == 1 ? MHD_RESPMEM_MUST_FREE : MHD_RESPMEM_PERSISTENT);
        assert (http_response);
    }
    else {
        http_response  = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
        assert (http_response);
    }

    for (const char* value = (const char*) zhash_first (headers); value != NULL; value = (const char*)zhash_next (headers)) {
        const char* key = zhash_cursor (headers);

        MHD_add_response_header (http_response, key, value);
    }
    zhash_destroy (&headers);

    // Removing from pending connections, which will resume the connection as well
    void* handle = zlistx_find (self->pending_connections, connection);
    assert (handle);
    zlistx_delete (self->pending_connections, handle);

    MHD_queue_response (connection, status_code, http_response);
    MHD_destroy_response (http_response);
}

//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

static void
server_actor (zsock_t *pipe, void *args)
{
    server_actor_t * self = server_actor_new (pipe, (zhttp_server_options_t *) args);
    if (!self) {
        assert (false);
        zsock_signal (pipe, 1);
        return;
    }

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    fd_set read_fd_set;
    fd_set write_fd_set;
    fd_set except_fd_set;
    FD_ZERO (&read_fd_set);
    FD_ZERO (&write_fd_set);
    FD_ZERO (&except_fd_set);

    SOCKET pipe_fd = zsock_fd (pipe);
    SOCKET backend_fd = zsock_fd (self->backend);

    int rc;

    while (!self->terminated) {
#if MHD_VERSION >= 0x00094400
        MHD_run_from_select (self->daemon, &read_fd_set, &write_fd_set, &except_fd_set);
#else
        MHD_run (self->daemon);
#endif

        bool use_timeout = true;

        if (zsock_has_in (self->pipe)) {
            s_recv_api (self);
            use_timeout = false;
        }

        while (zsock_has_in (self->backend)) {
            s_recv_backend (self);
            use_timeout = false;
        }

        FD_ZERO (&read_fd_set);
        FD_ZERO (&write_fd_set);
        FD_ZERO (&except_fd_set);
        FD_SET (pipe_fd, &read_fd_set);
        FD_SET (backend_fd, &read_fd_set);

        SOCKET max_fd = pipe_fd > backend_fd ? pipe_fd : backend_fd;
        rc = MHD_get_fdset (self->daemon, &read_fd_set, &write_fd_set, &except_fd_set, &max_fd);
        assert (rc == MHD_YES);

        unsigned long long timeout;
        rc = MHD_get_timeout (self->daemon, &timeout);

        struct timeval timeval;
        struct timeval *timeval_p = &timeval;

        if (!use_timeout) {
            timeval.tv_sec = 0;
            timeval.tv_usec = 0;
        }
        else if (rc == MHD_YES) {
            timeval.tv_sec = (long) (timeout / 1000);
            timeval.tv_usec = (long) (timeout % 1000);
        }
        else {
            timeval_p = NULL; // Not using any timeout
        }

        select (
                max_fd + 1,
                &read_fd_set,
                &write_fd_set,
                &except_fd_set,
                timeval_p);
    }
    server_actor_destroy (&self);
}

#endif

zhttp_server_t *
zhttp_server_new (zhttp_server_options_t *options) {
    zhttp_server_t *self = NULL;
#ifdef HAVE_LIBMICROHTTPD
    self = (zhttp_server_t *) zactor_new (server_actor, options);
#endif
    return self;
}

void
zhttp_server_destroy (zhttp_server_t **self_p) {
    zactor_destroy ((zactor_t **) self_p);
}


int
zhttp_server_port (zhttp_server_t *self) {
#ifdef HAVE_LIBMICROHTTPD
    assert (self);

    zstr_send (self, "PORT");
    int port;

    zsock_recv (self, "i", &port);
    return port;
#else
    return -1;
#endif
}

//  --------------------------------------------------------------------------
//  Self test of this actor.

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

void
zhttp_server_test (bool verbose)
{
#if defined(HAVE_LIBCURL) && defined(HAVE_LIBMICROHTTPD)
    printf (" * zhttp_server: ");

    //  @selftest
    int port = 40000 + (randof (10000));
    zhttp_server_options_t *options = zhttp_server_options_new ();
    zhttp_server_options_set_port (options, port);

    zhttp_server_t *server = zhttp_server_new (options);
    assert (server);
    //  @end

    char url[256];
    snprintf (url, 255, "http://127.0.0.1:%d", port);

    zhttp_client_t *self = zhttp_client_new (verbose);
    assert (self);

    zhttp_request_t* client_request = zhttp_request_new ();
    zhttp_request_set_url (client_request, url);
    zhttp_request_set_method (client_request, "POST");
    zhttp_request_set_content_const (client_request, "Hello!");
    int rc = zhttp_request_send (client_request, self, /*timeout*/ 10000, /* user args*/ NULL, NULL);
    assert (rc == 0);

    //  @selftest
    zsock_t *worker = zsock_new_dealer (zhttp_server_options_backend_address (options));
    zhttp_request_t *request = zhttp_request_new ();
    void *connection = zhttp_request_recv (request, worker);
    assert (connection);

    assert (streq (zhttp_request_method (request), "POST"));
    assert (streq (zhttp_request_url (request), "/"));
    assert (streq (zhttp_request_content (request), "Hello!"));

    zhttp_response_t *response = zhttp_response_new ();
    zhttp_response_set_content_const (response, "Welcome!");
    zhttp_response_set_status_code (response, 200);
    rc = zhttp_response_send (response, worker, &connection);
    assert (rc == 0);
    //  @end

    void *user_arg;
    void *user_arg2;
    zhttp_response_t* client_response = zhttp_response_new ();
    rc = zhttp_response_recv (client_response, self, &user_arg, &user_arg2);
    assert (rc == 0);
    assert (streq (zhttp_response_content (client_response), "Welcome!"));

    zhttp_client_destroy (&self);
    zhttp_request_destroy (&client_request);
    zhttp_response_destroy (&client_response);

    //  @selftest
    zhttp_request_destroy (&request);
    zhttp_response_destroy (&response);
    zsock_destroy (&worker);
    zhttp_server_destroy (&server);
    zhttp_server_options_destroy (&options);
    //  @end


    printf ("OK\n");
#endif
}
