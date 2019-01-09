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
    void *handler;
    void *arg;
    zchunk_t *response;
    CURL *curl;
    struct curl_slist *headers;
} http_request;

size_t write_data (void *buffer, size_t size, size_t nmemb, void *userp) {
    zchunk_t *chunk = (zchunk_t *) userp;
    zchunk_extend (chunk, buffer, nmemb);

    return nmemb;
}

static struct curl_slist *zlistx_to_slist (zlistx_t *zlist) {
    struct curl_slist *slist = NULL;

    char *header = zlistx_first (zlist);

    while (header) {
        slist = curl_slist_append (slist, header);
        header = zlistx_next (zlist);
    }

    return slist;
}

static void curl_destructor (CURL **curlp) {
    CURL *curl = *curlp;
    http_request *request;
    curl_easy_getinfo (curl, CURLINFO_PRIVATE, &request);

    zchunk_destroy (&request->response);
    curl_slist_free_all (request->headers);
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
    long timeout = 30;
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
            else if (streq (command, "GET")) {
                char *url;
                zlistx_t *headers;
                void *handler;
                void *arg;
                int rc = zsock_recv (pipe, "slpp", &url, &headers, &handler, &arg);
                assert (rc == 0);

                zchunk_t *response = zchunk_new (NULL, 100);
                assert (response);
                struct curl_slist *curl_headers = zlistx_to_slist (headers);
                CURL *curl = curl_easy_init ();
                zlistx_add_end (pending_handles, curl);
                http_request *request = (http_request *) zmalloc (sizeof (http_request));
                assert (request);
                request->handler = handler;
                request->arg = arg;
                request->curl = curl;
                request->response = response;
                request->headers = curl_headers;

                curl_easy_setopt (curl, CURLOPT_SHARE, share);
                curl_easy_setopt (curl, CURLOPT_TIMEOUT, timeout);
                curl_easy_setopt (curl, CURLOPT_VERBOSE, verbose);
                curl_easy_setopt (curl, CURLOPT_HTTPHEADER, curl_headers);
                curl_easy_setopt (curl, CURLOPT_URL, url);
                curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt (curl, CURLOPT_WRITEDATA, response);
                curl_easy_setopt (curl, CURLOPT_PRIVATE, request);

                code = curl_multi_add_handle (multi, curl);
                assert (code == CURLM_OK);
                zlistx_destroy (&headers);
                zstr_free (&url);
            } else if (streq (command, "POST")) {

                char *url;
                zlistx_t *headers;
                zchunk_t *body;
                void *handler;
                void *arg;
                int rc = zsock_recv (pipe, "slcpp", &url, &headers, &body, &handler, &arg);
                assert (rc == 0);

                zchunk_t *response = zchunk_new (NULL, 100);
                assert (response);
                struct curl_slist *curl_headers = zlistx_to_slist (headers);
                CURL *curl = curl_easy_init ();
                zlistx_add_end (pending_handles, curl);
                http_request *request = (http_request *) zmalloc (sizeof (http_request));
                assert (request);
                request->handler = handler;
                request->arg = arg;
                request->curl = curl;
                request->response = response;
                request->headers = curl_headers;

                curl_easy_setopt (curl, CURLOPT_SHARE, share);
                curl_easy_setopt (curl, CURLOPT_TIMEOUT, timeout);
                curl_easy_setopt (curl, CURLOPT_VERBOSE, verbose);
                curl_easy_setopt (curl, CURLOPT_HTTPHEADER, curl_headers);
                curl_easy_setopt (curl, CURLOPT_POSTFIELDS, zchunk_data (body));
                curl_easy_setopt (curl, CURLOPT_POSTFIELDSIZE, zchunk_size (body));
                curl_easy_setopt (curl, CURLOPT_URL, url);
                curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt (curl, CURLOPT_WRITEDATA, response);
                curl_easy_setopt (curl, CURLOPT_PRIVATE, request);

                code = curl_multi_add_handle (multi, curl);
                assert (code == CURLM_OK);
                zlistx_destroy (&headers);
                zstr_free (&url);
            } else {
                puts ("E: invalid message to actor");
                assert (false);
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
                http_request *request;
                curl_easy_getinfo (curl, CURLINFO_PRIVATE, &request);

                long response_code_long;
                curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &response_code_long);
                int response_code = (int) response_code_long;

                int rc = zsock_send (pipe, "icpp", response_code, request->response, request->handler, request->arg);
                assert (rc == 0);

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
//  Send a get request to the url, headers is optional.
//  Use userp to identify response when making multiple requests simultaneously.


int
zhttp_client_get (zhttp_client_t *self, const char *url, zlistx_t *headers, zhttp_client_fn handler, void *arg) {
#ifdef HAVE_LIBCURL
    if (headers)
        return zsock_send (self, "sslpp", "GET", url, headers, handler, arg);
    else
        return zsock_send (self, "sszpp", "GET", url, handler, arg);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Send a post request to the url, headers is optional.
//  Use arg to identify response when making multiple requests simultaneously.

int
zhttp_client_post (zhttp_client_t *self, const char *url, zlistx_t *headers, zchunk_t *body, zhttp_client_fn handler,
                   void *arg) {
#ifdef HAVE_LIBCURL

    if (headers && body)
        return zsock_send (self, "sslcpp", "POST", url, headers, body, handler, arg);
    else if (headers && !body)
        return zsock_send (self, "sslzpp", "POST", url, headers, handler, arg);
    else if (!headers && body)
        return zsock_send (self, "sszcpp", "POST", url, body, handler, arg);
    else
        return zsock_send (self, "sszzpp", "POST", url, handler, arg);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Invoke callback function for received responses.
//  Should be call after zpoller wait method.
//  Returns 0 if OK, -1 on failure.

int
zhttp_client_execute (zhttp_client_t *self) {
#ifdef HAVE_LIBCURL
    int response_code;
    zchunk_t *data;
    void *arg;
    zhttp_client_fn *handler;
    int rc;

    int events = zsock_events (self);

    while (zsock_has_in (self)) {
        rc = zsock_recv (self, "icpp", &response_code, &data, &handler, &arg);

        if (rc < 0)
            return rc;

        handler (arg, response_code, data);

        events = zsock_events (self);
    }

    return 0;
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Wait until a response is ready to be consumed.
//  Use when you need a synchronize response.
//
//  The timeout should be zero or greater, or -1 to wait indefinitely.
//
//  Returns 0 if a response is ready, -1 and otherwise. errno will be set to EAGAIN if no response is ready.

int
zhttp_client_wait (zhttp_client_t *self, int timeout) {
    zpoller_t *poller = zpoller_new (self, NULL);
    void* sock = zpoller_wait (poller, timeout);

    if (sock)
        return 0;

    if (zpoller_expired (poller))
        errno = EAGAIN;
    else
        errno = ETERM;

    zpoller_destroy (&poller);

    return -1;
}


//  --------------------------------------------------------------------------
//  Self test of this class

static void
test_handler (void *arg, int response_code, zchunk_t *data) {
    (void) response_code;

    bool *event = (bool *) arg;
    *event = true;

    assert (zchunk_streq (data, "Hello"));

    zchunk_destroy (&data);
}

void
zhttp_client_test (bool verbose) {
#if defined(HAVE_LIBCURL) && defined(ZMQ_STREAM)
    printf (" * zhttp_client: ");
    zsock_t *server = zsock_new_stream (NULL);
    int port = zsock_bind (server, "tcp://127.0.0.1:*");
    char url[255];
    sprintf (url, "http://127.0.0.1:%d", port);

    //  @selftest
    //  Simple create/destroy test
    zhttp_client_t *self = zhttp_client_new (verbose);
    assert (self);

    //  Send the get request
    bool event = false;
    zlistx_t *headers = zlistx_new ();
    zlistx_add_end (headers, "Host: zeromq.org");
    zhttp_client_get (self, url, headers, test_handler, &event);
    zlistx_destroy (&headers);

    //  Receive request on the server
    zchunk_t *routing_id;
    char *request;
    int rc = zsock_recv (server, "cs", &routing_id, &request);
    assert (rc == 0);

    //  Send the response
    char* response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
    zsock_send (server, "cs", routing_id, response);
    zchunk_destroy (&routing_id);
    zstr_free (&request);

    //  Receive the response on the http client
    rc = zhttp_client_wait (self, -1);
    assert (rc == 0);
    zhttp_client_execute (self);
    assert (event);

    // Send a POST request
    event = false;
    zchunk_t *body = zchunk_new ("World", 5);
    zhttp_client_post (self, url, NULL, body, test_handler, &event);

    //  Receive request on the server
    rc = zsock_recv (server, "cs", &routing_id, &request);
    assert (rc == 0);

    //  Send the response
    zsock_send (server, "cs", routing_id, response);
    zchunk_destroy (&routing_id);
    zstr_free (&request);

    //  Receive the response on the http client
    rc = zhttp_client_wait (self, -1);
    assert (rc == 0);
    zhttp_client_execute (self);
    assert (event);

    //  Sending another request, without being answer
    //  Checking the client ability to stop while request are inprogres
    zhttp_client_get (self, url, NULL, test_handler, NULL);

    zchunk_destroy (&body);
    zchunk_destroy (&routing_id);
    zstr_free (&request);
    zhttp_client_destroy (&self);
    zsock_destroy (&server);

    //  @end
    printf ("OK\n");
#endif
}
