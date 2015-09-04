/*  =========================================================================
    zdispatcher - run a steerable dispatcher in the background

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZDISPATCHER_H_INCLUDED
#define ZDISPATCHER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


//  @interface
//  Create new zdispatcher actor instance. The dispatcher forwards messages
//  based on an route (e.g. URL or URI) to a handler that has itself
//  register to a route; use the ENDPOINT command to configure the interface
//  for clients and handler:
//
//      zactor_t *dispatcher = zactor_new (zdispatcher, NULL);
//
//  Destroy zdispatcher instance. This destroys the the endpoint and stops any
//  message flow between clients and handlers:
//
//      zactor_destroy (&dispatcher);
//
//  Note that all zdispatcher commands are synchronous, so your application always
//  waits for a signal from the actor after each command.
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send  (dispatcher, "VERBOSE");
//      zsock_wait (dispatcher);
//
//  Attach the endpoint socket to endpoints, see zsock_attach ().
//
//      zstr_sendx (dispatcher, "ENDPOINT", endpoints, NULL);
//      zsock_wait (dispatcher);
//
//  Register a handler to a route. A route can contain regular expressions.
//  Note if a route contains parameterized regular expressions the parameters
//  are prepended to a dispatched message as a packed zhashx frame in front of
//  the actual message, see ztrie_insert_route ().
//
//      zsock_t *handler = zsock_new_dealer (">inproc://dispatching");
//      zstr_sendx (handler, "REGISTER", "/foo/{[^/]+}", NULL);
//      zsock_wait (handler);
//
//  Unregister a handler from a route. The route must match the registered one.
//
//      zstr_sendx (handler, "UNREGISTER", "/foo/{[^/]+}", NULL);
//      zsock_wait (handler);
//
//  Dispatch a message to a registered handler for specific route.
//
//      zsock_t *client = zsock_new_dealer (">inproc://dispatching");
//      zstr_sendx (client, "DISPATCH", "/bar/foo", "This is my message!", NULL);
//      zsock_wait (client);
//
//  Pause the dispatcher. A paused dispatcher will cease processing messages, causing
//  them to be queued up and potentially hit the high-water mark on the
//  endpoints socket, causing messages to be dropped, or writing
//  applications to block:
//
//      zstr_sendx (dispatcher, "PAUSE", NULL);
//      zsock_wait (dispatcher);
//
//  Resume the dispatcher. Note that the dispatcher starts automatically as soon as it
//  has a properly attached endpoints socket:
//
//      zstr_sendx (dispatcher, "RESUME", NULL);
//      zsock_wait (dispatcher);
//
//  This is the zdispatcher constructor as a zactor_fn.
CZMQ_EXPORT void
    zdispatcher (zsock_t *pipe, void *args);

//  Self test of this actor
CZMQ_EXPORT void
    zdispatcher_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
