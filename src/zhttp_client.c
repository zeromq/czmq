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
    void *userp;
    zchunk_t *data;
    CURL *curl;
    struct curl_slist *headers;
} http_request;

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
    zchunk_t *chunk = (zchunk_t *) userp;
    zchunk_extend (chunk, buffer, nmemb);

    return nmemb;
}

static struct curl_slist * zlistx_to_slist (zlistx_t *zlist) {
    struct curl_slist *slist = NULL;

    char *header = zlistx_first (zlist);

    while (header) {
        slist = curl_slist_append(slist, header);
        header = zlistx_next (zlist);
    }

    return slist;
}

static void curl_destructor (CURL **curlp) {
    CURL *curl = *curlp;
    http_request *request;
    curl_easy_getinfo(curl, CURLINFO_PRIVATE, &request);

    zchunk_destroy (&request->data);
    curl_slist_free_all (request->headers);
    curl_easy_cleanup (curl);
    free (request);
    *curlp = NULL;
}

static void handler (zsock_t *pipe, void *args) {
    curl_global_init(CURL_GLOBAL_ALL);
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
        int events = zsock_events (pipe);
        if ((events & ZMQ_POLLIN) == 0) {
            code = curl_multi_wait (multi, &waitfd, 1, 1000, NULL);
            assert (code == CURLM_OK);
        }

        events = zsock_events (pipe);
        if (events & ZMQ_POLLIN) {
            char* command = zstr_recv (pipe);
            if (!command)
                break;          //  Interrupted

            //  All actors must handle $TERM in this way
            if (streq (command, "$TERM"))
                terminated = true;
            else if (streq (command, "GET")) {
                char *url;
                zlistx_t *headers;
                void *userp;
                int rc = zsock_recv (pipe, "slp", &url, &headers, &userp);
                assert (rc == 0);

                zchunk_t *data = zchunk_new (NULL, 100);
                assert (data);
                struct curl_slist *curl_headers = zlistx_to_slist (headers);
                CURL *curl = curl_easy_init ();
                zlistx_add_end (pending_handles, curl);
                http_request *request = (http_request *) zmalloc (sizeof (http_request));
                assert (request);
                request->userp = userp;
                request->curl = curl;
                request->data = data;
                request->headers = curl_headers;

                curl_easy_setopt (curl, CURLOPT_SHARE, share);
                curl_easy_setopt (curl, CURLOPT_TIMEOUT, timeout);
                curl_easy_setopt (curl, CURLOPT_VERBOSE, verbose);
                curl_easy_setopt (curl, CURLOPT_HTTPHEADER, curl_headers);
                curl_easy_setopt (curl, CURLOPT_URL, url);
                curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt (curl, CURLOPT_WRITEDATA, data);
                curl_easy_setopt (curl, CURLOPT_PRIVATE, request);

                code = curl_multi_add_handle (multi, curl);
                assert (code == CURLM_OK);
                zlistx_destroy (&headers);
                zstr_free (&url);
           }
           else {
               puts ("E: invalid message to actor");
               assert (false);
           }
           zstr_free (&command);
        }

        int still_running;
        code = curl_multi_perform (multi, &still_running);
        assert (code == CURLM_OK);

        int msgq = 0;
        struct CURLMsg *msg = curl_multi_info_read(multi, &msgq);

        while (msg) {
            if(msg->msg == CURLMSG_DONE) {
                CURL *curl = msg->easy_handle;
                http_request *request;
                curl_easy_getinfo(curl, CURLINFO_PRIVATE, &request);

                long response_code_long;
                curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &response_code_long);
                int response_code = (int)response_code_long;

                int rc = zsock_send (pipe, "icp", response_code, request->data, request->userp);
                assert (rc == 0);

                curl_multi_remove_handle (multi, curl);

                //  Remove curl from the pending handles and delete it
                void *handle = zlistx_find (pending_handles, curl);
                assert (handle);
                rc = zlistx_delete (pending_handles, handle);
                assert (rc == 0);
            }

            msg = curl_multi_info_read(multi, &msgq);
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
zhttp_client_new (bool verbose)
{
    zhttp_client_t *self = NULL;
#ifdef HAVE_LIBCURL
    self = (zhttp_client_t *) zactor_new(handler, &verbose);
#endif
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zhttp_client

void
zhttp_client_destroy (zhttp_client_t **self_p)
{
    zactor_destroy ((zactor_t **)self_p);
}


//  --------------------------------------------------------------------------
//  Send a get request to the url, headers is optional.
//  Use userp to identify response when making multiple requests simultaneously.


int
zhttp_client_get (zhttp_client_t *self, const char *url, zlistx_t *headers, void *userp) {
#ifdef HAVE_LIBCURL
    if (headers)
        return zsock_send (self, "sslp", "GET", url, headers, userp);
    else
        return zsock_send (self, "sszp", "GET", url, userp);
#else
    return -1;
#endif
}

//  --------------------------------------------------------------------------
//  Receive the response for one of the requests. Blocks until a response is ready.
//  Use userp to identify the request.
int
zhttp_client_recv (zhttp_client_t *self, int *response_code, zchunk_t **data, void **userp) {
#ifdef HAVE_LIBCURL
    return zsock_recv (self, "icp", response_code, data, userp);
#else
    return -1;
#endif
}

//  --------------------------------------------------------------------------
//  Self test of this class

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
zhttp_client_test (bool verbose)
{
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
    zlistx_t *headers = zlistx_new ();
    zlistx_add_end (headers, "Host: zeromq.org");
    zhttp_client_get (self, url, headers, NULL);
    zlistx_destroy (&headers);

    //  Receive request on the server
    zchunk_t *routing_id;
    char *request;
    int rc = zsock_recv (server, "cs", &routing_id, &request);
    assert (rc == 0);

    //  Send the response
    char* response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
    zsock_send (server, "cs", routing_id, response);

    //  Receive the response on the http client
    int code;
    zchunk_t *data;

    zhttp_client_recv (self, &code, &data, NULL);
    assert (zchunk_streq (data, "Hello"));

    //  Sending another request, without being answer
    //  Checking the client ability to stop while request are inprogres
    zhttp_client_get (self, url, NULL, NULL);

    zchunk_destroy (&data);
    zchunk_destroy (&routing_id);
    zstr_free (&request);
    zhttp_client_destroy (&self);
    zsock_destroy (&server);

    //  @end
    printf ("OK\n");
#endif
}
