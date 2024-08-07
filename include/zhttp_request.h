/*  =========================================================================
    zhttp_request - Http request that can be received from zhttp_server or sent to zhttp_client.
Class can be reused between send & recv calls.
Headers and Content is being destroyed after every send call.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZHTTP_REQUEST_H_INCLUDED
#define ZHTTP_REQUEST_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @warning THE FOLLOWING @INTERFACE BLOCK IS AUTO-GENERATED BY ZPROJECT
//  @warning Please edit the model at "api/zhttp_request.api" to make changes.
//  @interface
//  This is a draft class, and may change without notice. It is disabled in
//  stable builds by default. If you use this in applications, please ask
//  for it to be pushed to stable state. Use --enable-drafts to enable.
#ifdef CZMQ_BUILD_DRAFT_API
//  *** Draft method, for development use, may change without warning ***
//  Create a new http request.
CZMQ_EXPORT zhttp_request_t *
    zhttp_request_new (void);

//  *** Draft method, for development use, may change without warning ***
//  Destroy an http request.
CZMQ_EXPORT void
    zhttp_request_destroy (zhttp_request_t **self_p);

//  *** Draft method, for development use, may change without warning ***
//  Receive a new request from zhttp_server.
//  Return the underlying connection if successful, to be used when calling zhttp_response_send.
CZMQ_EXPORT void *
    zhttp_request_recv (zhttp_request_t *self, zsock_t *sock);

//  *** Draft method, for development use, may change without warning ***
//  Send a request to zhttp_client.
//  Url and the request path will be concatenated.
//  This behavior is useful for url rewrite and reverse proxy.
//
//  Send also allow two user provided arguments which will be returned with the response.
//  The reason for two, is to be able to pass around the server connection when forwarding requests or both a callback function and an arg.
CZMQ_EXPORT int
    zhttp_request_send (zhttp_request_t *self, zhttp_client_t *client, int timeout, void *arg, void *arg2);

//  *** Draft method, for development use, may change without warning ***
//  Get the request method
CZMQ_EXPORT const char *
    zhttp_request_method (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Set the request method
CZMQ_EXPORT void
    zhttp_request_set_method (zhttp_request_t *self, const char *method);

//  *** Draft method, for development use, may change without warning ***
//  Get the request url.
//  When receiving a request from http server this is only the path part of the url.
CZMQ_EXPORT const char *
    zhttp_request_url (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Set the request url
//  When sending a request to http client this should be full url.
CZMQ_EXPORT void
    zhttp_request_set_url (zhttp_request_t *self, const char *url);

//  *** Draft method, for development use, may change without warning ***
//  Get the request content type
CZMQ_EXPORT const char *
    zhttp_request_content_type (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Set the request content type
CZMQ_EXPORT void
    zhttp_request_set_content_type (zhttp_request_t *self, const char *content_type);

//  *** Draft method, for development use, may change without warning ***
//  Get the content length of the request
CZMQ_EXPORT size_t
    zhttp_request_content_length (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Get the headers of the request
CZMQ_EXPORT zhash_t *
    zhttp_request_headers (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Get the content of the request.
CZMQ_EXPORT const char *
    zhttp_request_content (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Get the content of the request.
//  Caller owns return value and must destroy it when done.
CZMQ_EXPORT char *
    zhttp_request_get_content (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Set the content of the request.
//  Content must by dynamically allocated string.
//  Takes ownership of the content.
CZMQ_EXPORT void
    zhttp_request_set_content (zhttp_request_t *self, char **content);

//  *** Draft method, for development use, may change without warning ***
//  Set the content of the request..
//  The content is assumed to be constant-memory and will therefore not be copied or deallocated in any way.
CZMQ_EXPORT void
    zhttp_request_set_content_const (zhttp_request_t *self, const char *content);

//  *** Draft method, for development use, may change without warning ***
//  Set the content to NULL
CZMQ_EXPORT void
    zhttp_request_reset_content (zhttp_request_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Set the request username
CZMQ_EXPORT void
    zhttp_request_set_username (zhttp_request_t *self, const char *username);

//  *** Draft method, for development use, may change without warning ***
//  Set the request password
CZMQ_EXPORT void
    zhttp_request_set_password (zhttp_request_t *self, const char *password);

//  *** Draft method, for development use, may change without warning ***
//  Match the path of the request.
//  Support wildcards with '%s' symbol inside the match string.
//  Matching wildcards until the next '/', '?' or '\0'.
//  On successful match the variadic arguments will be filled with the matching strings.
//  On successful match the method is modifying the url field and break it into substrings.
//  If you need to use the url, do it before matching or take a copy.
//
//  User must not free the variadic arguments as they are part of the url.
//
//  To use the percent symbol, just double it, e.g "%%something".
//
//  Example:
//  if (zhttp_request_match (request, "POST", "/send/%s/%s", &name, &id))
CZMQ_EXPORT bool
    zhttp_request_match (zhttp_request_t *self, const char *method, const char *path, ...);

//  *** Draft method, for development use, may change without warning ***
//  Self test of this class.
CZMQ_EXPORT void
    zhttp_request_test (bool verbose);

#endif // CZMQ_BUILD_DRAFT_API
//  @end

#ifdef __cplusplus
}
#endif

#endif
