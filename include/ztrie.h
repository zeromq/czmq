/*  =========================================================================
    ztrie - simple trie for tokenizable strings

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZTRIE_H_INCLUDED
#define ZTRIE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// Callback function for ztrie_node to destroy node data
typedef void (ztrie_destroy_data_fn) (void **data);

//  @interface
//  Creates a new ztrie.
CZMQ_EXPORT ztrie_t *
    ztrie_new (char delimiter);

//  Destroy the ztrie.
CZMQ_EXPORT void
    ztrie_destroy (ztrie_t **self_p);

//  Inserts a new route into the tree and attaches the data. Returns -1
//  if the route already exists, otherwise 0. This method takes ownership of
//  the provided data if a destroy_data_fn is provided.
CZMQ_EXPORT int
    ztrie_insert_route (ztrie_t *self, char *path, void *data, ztrie_destroy_data_fn *destroy_data_fn);

//  Removes a route from the trie and destroys its data. Returns -1 if the
//  route does not exists, otherwise 0.
CZMQ_EXPORT int
   ztrie_remove_route (ztrie_t *self, char *path);

//  Returns true if the path matches a route in the tree, otherwise false.
CZMQ_EXPORT bool
    ztrie_matches (ztrie_t *self, char *path);

//  Returns the data of a matched route from last ztrie_matches. If the path
//  did not match, returns NULL. Do not delete the data as it's owned by
//  ztrie.
CZMQ_EXPORT void *
    ztrie_hit_data (ztrie_t *self);

//  Returns the count of parameters that a matched route has.
CZMQ_EXPORT size_t
ztrie_hit_parameter_count (ztrie_t *self);

//  Returns the parameters of a matched route with named regexes from last
//  ztrie_matches. If the path did not match or the route did not contain any
//  named regexes, returns NULL.
CZMQ_EXPORT zhashx_t *
    ztrie_hit_parameters (ztrie_t *self);

//  Print properties of object
CZMQ_EXPORT void
    ztrie_print (ztrie_t *self);

//  Self test of this class
CZMQ_EXPORT void
    ztrie_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
