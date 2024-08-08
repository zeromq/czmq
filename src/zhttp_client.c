/*  =========================================================================
    zhttp_client - provides a simple http client

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
    zhttp_client - provides a simple http client
@discuss
@end
*/

#include "czmq_classes.h"

#ifdef HAVE_LIBCURL

#include <curl/curl.h>

#endif

#ifdef HAVE_LIBCURL

typedef struct {
    CURL *curl;
    void *arg;
    void *arg2;

    char *request_content;
    bool free_request_content;

    struct curl_slist *request_headers;

    char *response_content;
    size_t response_content_length;
    zhash_t *response_headers;

} http_request_t;

size_t header_callback(char *buffer, size_t size, size_t nitems, void *userp) {
    http_request_t *request = (http_request_t *) userp;

    // TODO: support multiline headers

    char *colon = buffer;
    const char *end = buffer + (size * nitems);

    while (colon != end) {
        if (*colon == ':') {
            *colon = '\0';
            colon++;
            char* key = buffer;
            char* value = colon;
            char *value_end = colon;

            // Find the end of line and set it to null terminated
            while (*value_end != '\r' && *value_end != '\n' && value_end != end) {
                value_end++;
            }
            *value_end = '\0';

            // Skip any whitespace at the beginning of the value
            while (*value == ' ' || *value == '\t') {
                if (value == value_end)
                    break;
                value++;
            }

            zhash_insert (request->response_headers, key, value);

            break;
        }

        colon++;
    }

    return size * nitems;
}

size_t write_data (void *buffer, size_t size, size_t nmemb, void *userp) {
    http_request_t *request = (http_request_t *) userp;

    size_t offset = 0;

    if (request->response_content == NULL) {
        request->response_content_length = nmemb;
        request->response_content = (char*)malloc (request->response_content_length + 1);
    }
    else {
        offset = request->response_content_length;
        request->response_content_length += nmemb;
        request->response_content = (char*) realloc (request->response_content, request->response_content_length + 1);
    }

    memcpy (request->response_content + offset, buffer, nmemb);
    request->response_content[request->response_content_length] = '\0';

    return nmemb;
}

static struct curl_slist *zhash_to_slist (zhash_t *headers) {
    struct curl_slist *slist = NULL;

    char *value = (char*) zhash_first (headers);

    while (value) {
        const char *key = zhash_cursor (headers);
        char *header = zsys_sprintf ("%s: %s", key, value); // TODO: avoid allocation here
        slist = curl_slist_append (slist, header);
        zstr_free (&header);

        value = (char *) zhash_next (headers);
    }

    return slist;
}

static void curl_destructor (CURL **curlp) {
    CURL *curl = *curlp;
    http_request_t *request;
    curl_easy_getinfo (curl, CURLINFO_PRIVATE, &request);

    if (request->free_request_content)
        zstr_free (&request->request_content);

    // Those will usually be null, as we are passing the pointer to the caller
    zstr_free (&request->response_content);
    zhash_destroy (&request->response_headers);

    curl_slist_free_all (request->request_headers);
    curl_easy_cleanup (curl);
    free (request);
    *curlp = NULL;
}

static void zhttp_client_actor (zsock_t *pipe, void *args) {
    curl_global_init (CURL_GLOBAL_ALL);
    CURLM *multi = curl_multi_init ();
    CURLSH *share = curl_share_init ();
    curl_share_setopt (share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    curl_share_setopt (share, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
    curl_share_setopt (share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);

    long verbose = (*(bool *) args) ? 1L : 0L;
    CURLMcode code;

    SOCKET pipefd = zsock_fd (pipe);
    struct curl_waitfd waitfd = {pipefd, CURL_WAIT_POLLIN};

    //  List to hold pending curl handles, in case we are destroy the client
    //  while request are inprogress
    zlistx_t *pending_handles = zlistx_new ();
    zlistx_set_destructor (pending_handles, (zlistx_destructor_fn *) curl_destructor);

    zsock_signal (pipe, 0);

    bool terminated = false;
    while (!terminated) {
        if (!zsock_has_in (pipe)) {
            code = curl_multi_wait (multi, &waitfd, 1, 1000, NULL);
            assert (code == CURLM_OK);
        }

        if (zsock_has_in (pipe)) {
            char *command = zstr_recv (pipe);
            if (!command)
                break;          //  Interrupted

            //  All actors must handle $TERM in this way
            if (streq (command, "$TERM"))
                terminated = true;
            else {
                int timeout;
                void *arg;
                void *arg2;
                char *url;
                zhash_t *headers;
                byte free_content;
                char *content;
                char *username;
                char *password;

                int rc = zsock_brecv (pipe, "4ppSp1pss", &timeout, &arg, &arg2,
                        &url, &headers, &free_content, &content, &username, &password);
                assert (rc == 0);

                struct curl_slist *curl_headers = zhash_to_slist (headers);

                CURL *curl = curl_easy_init ();
                zlistx_add_end (pending_handles, curl);
                http_request_t *request = (http_request_t *) zmalloc (sizeof (http_request_t));
                assert (request);

                request->arg = arg;
                request->arg2 = arg2;
                request->curl = curl;
                request->request_headers = curl_headers;
                request->request_content = content;
                request->free_request_content = free_content;
                request->response_headers = zhash_new ();
                zhash_autofree (request->response_headers);

                curl_easy_setopt (curl, CURLOPT_SHARE, share);
                curl_easy_setopt (curl, CURLOPT_TIMEOUT_MS, (long)timeout);
                curl_easy_setopt (curl, CURLOPT_VERBOSE, verbose);
                curl_easy_setopt (curl, CURLOPT_HTTPHEADER, curl_headers);
                curl_easy_setopt (curl, CURLOPT_URL, url);
                curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt (curl, CURLOPT_WRITEDATA, request);
                curl_easy_setopt (curl, CURLOPT_HEADERFUNCTION, header_callback);
                curl_easy_setopt (curl, CURLOPT_HEADERDATA, request);
                curl_easy_setopt (curl, CURLOPT_PRIVATE, request);

                if (*username)
                    curl_easy_setopt (curl, CURLOPT_USERNAME, username);

                if (*password)
                    curl_easy_setopt (curl, CURLOPT_PASSWORD, password);

                if (streq (command, "POST") || streq (command, "PUT") ||
                      streq (command, "PATCH")) {
                    curl_easy_setopt (curl, CURLOPT_POSTFIELDS, content);
                    curl_easy_setopt (curl, CURLOPT_POSTFIELDSIZE, content ? strlen (content) : 0);

                    if (strneq (command, "POST"))
                        curl_easy_setopt (curl, CURLOPT_CUSTOMREQUEST, command);
                }

                code = curl_multi_add_handle (multi, curl);
                assert (code == CURLM_OK);
                zhash_destroy (&headers);
                zstr_free (&url);

            }
            zstr_free (&command);
        }

        int still_running;
        code = curl_multi_perform (multi, &still_running);
        assert (code == CURLM_OK);

        int msgq = 0;
        struct CURLMsg *msg = curl_multi_info_read (multi, &msgq);

        while (msg) {
            if (msg->msg == CURLMSG_DONE) {
                CURL *curl = msg->easy_handle;
                http_request_t *request;
                curl_easy_getinfo (curl, CURLINFO_PRIVATE, &request);

                long response_code_long;
                curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &response_code_long);
                uint32_t response_code = (uint32_t) response_code_long;

                uint32_t result = 0;

                // TODO: more mappings
                switch (msg->data.result) {
                    case CURLE_OK:
                        result = 0;
                        break;
                    case CURLE_OPERATION_TIMEDOUT:
                        result = ETIMEDOUT;
                        break;
                    default:
                        result = msg->data.result;
                }

                int rc = zsock_bsend (pipe, "4pp4pp", result, request->arg, request->arg2, response_code, request->response_headers, request->response_content);
                assert (rc == 0);

                request->response_headers = NULL;
                request->response_content = NULL;

                curl_multi_remove_handle (multi, curl);

                //  Remove curl from the pending handles and delete it
                void *handle = zlistx_find (pending_handles, curl);
                assert (handle);
                rc = zlistx_delete (pending_handles, handle);
                assert (rc == 0);
            }

            msg = curl_multi_info_read (multi, &msgq);
        }
    }

    zlistx_destroy (&pending_handles);
    curl_share_cleanup (share);
    curl_multi_cleanup (multi);
    curl_global_cleanup ();
}

#endif

//  --------------------------------------------------------------------------
//  Create a new zhttp_client

zhttp_client_t *
zhttp_client_new (bool verbose) {
    zhttp_client_t *self = NULL;
#ifdef HAVE_LIBCURL
    self = (zhttp_client_t *) zactor_new (zhttp_client_actor, &verbose);
#endif
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zhttp_client

void
zhttp_client_destroy (zhttp_client_t **self_p) {
    zactor_destroy ((zactor_t **) self_p);
}


//  --------------------------------------------------------------------------
//  Self test of this class


void
zhttp_client_test (bool verbose) {
#if defined(HAVE_LIBCURL) && defined(HAVE_LIBMICROHTTPD)
    printf (" * zhttp_client: ");

    int port = 40000 + (randof (10000));
    zhttp_server_options_t *options = zhttp_server_options_new ();
    zhttp_server_options_set_port (options, port);

    zhttp_server_t *server = zhttp_server_new (options);
    assert (server);

    char url[256];
    snprintf (url, 255, "http://127.0.0.1:%d", port);

    //  @selftest
    zhttp_client_t *self = zhttp_client_new (verbose);
    assert (self);

    zhttp_request_t* request = zhttp_request_new ();
    zhttp_request_set_url (request, url);
    zhttp_request_set_method (request, "GET");
    int rc = zhttp_request_send (request, self, /*timeout*/ 10000, /* user args*/ NULL, NULL);
    assert (rc == 0);

    //  @end

    zsock_t *worker = zsock_new_dealer (zhttp_server_options_backend_address (options));
    zhttp_request_t *server_request = zhttp_request_new ();
    void *connection = zhttp_request_recv (server_request, worker);
    assert (connection);

    zhttp_response_t *server_response = zhttp_response_new ();
    zhttp_response_set_content_const (server_response, "Hello World!");
    zhttp_response_set_status_code (server_response, 200);
    rc = zhttp_response_send (server_response, worker, &connection);
    assert (rc == 0);

    //  @selftest
    void *user_arg;
    void *user_arg2;
    zhttp_response_t* response = zhttp_response_new ();
    rc = zhttp_response_recv (response, self, &user_arg, &user_arg2);
    assert (rc == 0);
    assert (streq (zhttp_response_content (response), "Hello World!"));

    zhttp_client_destroy (&self);
    zhttp_request_destroy (&request);
    zhttp_response_destroy (&response);

    //  @end

    zhttp_request_destroy (&server_request);
    zhttp_response_destroy (&server_response);
    zsock_destroy (&worker);
    zhttp_server_destroy (&server);
    zhttp_server_options_destroy (&options);

    printf ("OK\n");
#endif
}
