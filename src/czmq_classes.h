/*  =========================================================================
    czmq_classes - private header file

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef CZMQ_CLASSES_H_INCLUDED
#define CZMQ_CLASSES_H_INCLUDED

//  Platform definitions, must come first
#include "platform.h"

//  External API
#include "../include/czmq.h"

//  Extra headers

//  Opaque class structures to allow forward references
#ifndef ZGOSSIP_MSG_T_DEFINED
typedef struct _zgossip_msg_t zgossip_msg_t;
#define ZGOSSIP_MSG_T_DEFINED
#endif

//  Internal API

#include "zgossip_msg.h"

//  *** To avoid double-definitions, only define if building without draft ***
#ifndef CZMQ_BUILD_DRAFT_API

//  *** Draft callbacks, defined for internal use only ***
// Function to be called on zactor_destroy. Default behavior is to send zmsg_t with string "$TERM" in a first frame.
//
// An example - to send $KTHXBAI string
//
//     if (zstr_send (self->pipe, "$KTHXBAI") == 0)
//         zsock_wait (self->pipe);
typedef void (zactor_destructor_fn) (
    zactor_t *self);

//  *** Draft method, defined for internal use only ***
//  Change default destructor by custom function. Actor MUST be able to handle new message instead of default $TERM.
CZMQ_PRIVATE void
    zactor_set_destructor (zactor_t *self, zactor_destructor_fn destructor);

//  *** Draft method, defined for internal use only ***
//  Accepts public/secret key text pair from caller
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zcert_t *
    zcert_new_from_txt (const char *public_txt, const char *secret_txt);

//  *** Draft method, defined for internal use only ***
//  Unset certificate metadata.
CZMQ_PRIVATE void
    zcert_unset_meta (zcert_t *self, const char *name);

//  *** Draft callbacks, defined for internal use only ***
// Loaders retrieve certificates from an arbitrary source.
typedef void (zcertstore_loader) (
    zcertstore_t *self);
// Destructor for loader state.
typedef void (zcertstore_destructor) (
    void **self_p);

//  *** Draft method, defined for internal use only ***
//  Override the default disk loader with a custom loader fn.
CZMQ_PRIVATE void
    zcertstore_set_loader (zcertstore_t *self, zcertstore_loader loader, zcertstore_destructor destructor, void *state);

//  *** Draft method, defined for internal use only ***
//  Empty certificate hashtable. This wrapper exists to be friendly to bindings,
//  which don't usually have access to struct internals.
CZMQ_PRIVATE void
    zcertstore_empty (zcertstore_t *self);

//  *** Draft method, defined for internal use only ***
//  Return a list of all the certificates in the store.
//  The caller takes ownership of the zlistx_t object and is responsible
//  for destroying it.  The caller does not take ownership of the zcert_t
//  objects.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zlistx_t *
    zcertstore_certs (zcertstore_t *self);

//  *** Draft method, defined for internal use only ***
//  Destroy subtree (all children)
CZMQ_PRIVATE void
    zconfig_remove_subtree (zconfig_t *self);

//  *** Draft method, defined for internal use only ***
//  Destroy node and subtree (all children)
CZMQ_PRIVATE void
    zconfig_remove (zconfig_t **self_p);

//  *** Draft method, defined for internal use only ***
//  Create new temporary file for writing via tmpfile. File is automaticaly
//  deleted on destroy
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zfile_t *
    zfile_tmp (void);

//  *** Draft method, defined for internal use only ***
//  Return frame routing ID, if the frame came from a ZMQ_SERVER socket.
//  Else returns zero.
CZMQ_PRIVATE uint32_t
    zframe_routing_id (zframe_t *self);

//  *** Draft method, defined for internal use only ***
//  Set routing ID on frame. This is used if/when the frame is sent to a
//  ZMQ_SERVER socket.
CZMQ_PRIVATE void
    zframe_set_routing_id (zframe_t *self, uint32_t routing_id);

//  *** Draft method, defined for internal use only ***
//  Return frame group of radio-dish pattern.
CZMQ_PRIVATE const char *
    zframe_group (zframe_t *self);

//  *** Draft method, defined for internal use only ***
//  Set group on frame. This is used if/when the frame is sent to a
//  ZMQ_RADIO socket.
//  Return -1 on error, 0 on success.
CZMQ_PRIVATE int
    zframe_set_group (zframe_t *self, const char *group);

//  *** Draft method, defined for internal use only ***
//  Same as pack but uses a user-defined serializer function to convert items
//  into longstr.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zframe_t *
    zhashx_pack_own (zhashx_t *self, zhashx_serializer_fn serializer);

//  *** Draft method, defined for internal use only ***
//  Same as unpack but uses a user-defined deserializer function to convert
//  a longstr back into item format.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zhashx_t *
    zhashx_unpack_own (zframe_t *frame, zhashx_deserializer_fn deserializer);

//  *** Draft method, defined for internal use only ***
//  Get a list of network interfaces currently defined on the system
//  Includes IPv6 interfaces
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE ziflist_t *
    ziflist_new_ipv6 (void);

//  *** Draft method, defined for internal use only ***
//  Reload network interfaces from system, including IPv6
CZMQ_PRIVATE void
    ziflist_reload_ipv6 (ziflist_t *self);

//  *** Draft method, defined for internal use only ***
//  Return true if the current interface uses IPv6
CZMQ_PRIVATE bool
    ziflist_is_ipv6 (ziflist_t *self);

//  *** Draft method, defined for internal use only ***
//  Return message routing ID, if the message came from a ZMQ_SERVER socket.
//  Else returns zero.
CZMQ_PRIVATE uint32_t
    zmsg_routing_id (zmsg_t *self);

//  *** Draft method, defined for internal use only ***
//  Set routing ID on message. This is used if/when the message is sent to a
//  ZMQ_SERVER socket.
CZMQ_PRIVATE void
    zmsg_set_routing_id (zmsg_t *self, uint32_t routing_id);

//  *** Draft method, defined for internal use only ***
//  Create a SERVER socket. Default action is bind.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zsock_t *
    zsock_new_server (const char *endpoint);

//  *** Draft method, defined for internal use only ***
//  Create a CLIENT socket. Default action is connect.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zsock_t *
    zsock_new_client (const char *endpoint);

//  *** Draft method, defined for internal use only ***
//  Create a RADIO socket. Default action is bind.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zsock_t *
    zsock_new_radio (const char *endpoint);

//  *** Draft method, defined for internal use only ***
//  Create a DISH socket. Default action is connect.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zsock_t *
    zsock_new_dish (const char *endpoint);

//  *** Draft method, defined for internal use only ***
//  Create a GATHER socket. Default action is bind.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zsock_t *
    zsock_new_gather (const char *endpoint);

//  *** Draft method, defined for internal use only ***
//  Create a SCATTER socket. Default action is connect.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zsock_t *
    zsock_new_scatter (const char *endpoint);

//  *** Draft method, defined for internal use only ***
//  Return socket routing ID if any. This returns 0 if the socket is not
//  of type ZMQ_SERVER or if no request was already received on it.
CZMQ_PRIVATE uint32_t
    zsock_routing_id (zsock_t *self);

//  *** Draft method, defined for internal use only ***
//  Set routing ID on socket. The socket MUST be of type ZMQ_SERVER.
//  This will be used when sending messages on the socket via the zsock API.
CZMQ_PRIVATE void
    zsock_set_routing_id (zsock_t *self, uint32_t routing_id);

//  *** Draft method, defined for internal use only ***
//  Join a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
//  Returns 0 if OK, -1 if failed.
CZMQ_PRIVATE int
    zsock_join (void *self, const char *group);

//  *** Draft method, defined for internal use only ***
//  Leave a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
//  Returns 0 if OK, -1 if failed.
CZMQ_PRIVATE int
    zsock_leave (void *self, const char *group);

//  *** Draft method, defined for internal use only ***
//  De-compress and receive C string from socket, received as a message
//  with two frames: size of the uncompressed string, and the string itself.
//  Caller must free returned string using zstr_free(). Returns NULL if the
//  context is being terminated or the process was interrupted.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE char *
    zstr_recv_compress (void *source);

//  *** Draft method, defined for internal use only ***
//  Compress and send a C string to a socket, as a message with two frames:
//  size of the uncompressed string, and the string itself. The string is
//  sent without trailing null byte; to read this you can use
//  zstr_recv_compress, or a similar method that de-compresses and adds a
//  null terminator on the received string.
CZMQ_PRIVATE int
    zstr_send_compress (void *dest, const char *string);

//  *** Draft method, defined for internal use only ***
//  Compress and send a C string to a socket, as zstr_send_compress(),
//  with a MORE flag, so that you can send further strings in the same
//  multi-part message.
CZMQ_PRIVATE int
    zstr_sendm_compress (void *dest, const char *string);

//  *** Draft method, defined for internal use only ***
//  Accepts a void pointer and returns a fresh character string. If source
//  is null, returns an empty string.
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE char *
    zstr_str (void *source);

//  *** Draft method, defined for internal use only ***
//  Configure the threshold value of filesystem object age per st_mtime
//  that should elapse until we consider that object "stable" at the
//  current zclock_time() moment.
//  The default is S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC defined in zsys.c
//  which generally depends on host OS, with fallback value of 5000.
CZMQ_PRIVATE void
    zsys_set_file_stable_age_msec (int64_t file_stable_age_msec);

//  *** Draft method, defined for internal use only ***
//  Return current threshold value of file stable age in msec.
//  This can be used in code that chooses to wait for this timeout
//  before testing if a filesystem object is "stable" or not.
CZMQ_PRIVATE int64_t
    zsys_file_stable_age_msec (void);

//  *** Draft constants, defined for internal use only ***
#define ZGOSSIP_MSG_HELLO 1                 //
#define ZGOSSIP_MSG_PUBLISH 2               //
#define ZGOSSIP_MSG_PING 3                  //
#define ZGOSSIP_MSG_PONG 4                  //
#define ZGOSSIP_MSG_INVALID 5               //

//  *** Draft method, defined for internal use only ***
//  Create a new empty zgossip_msg
//  Caller owns return value and must destroy it when done.
CZMQ_PRIVATE zgossip_msg_t *
    zgossip_msg_new (void);

//  *** Draft method, defined for internal use only ***
//  Destroy a zgossip_msg instance
CZMQ_PRIVATE void
    zgossip_msg_destroy (zgossip_msg_t **self_p);

//  *** Draft method, defined for internal use only ***
//  Receive a zgossip_msg from the socket. Returns 0 if OK, -1 if
//  there was an error. Blocks if there is no message waiting.
CZMQ_PRIVATE int
    zgossip_msg_recv (zgossip_msg_t *self, zsock_t *input);

//  *** Draft method, defined for internal use only ***
//  Send the zgossip_msg to the output socket, does not destroy it
CZMQ_PRIVATE int
    zgossip_msg_send (zgossip_msg_t *self, zsock_t *output);

//  *** Draft method, defined for internal use only ***
//  Print contents of message to stdout
CZMQ_PRIVATE void
    zgossip_msg_print (zgossip_msg_t *self);

//  *** Draft method, defined for internal use only ***
//  Get the message routing id, as a frame
CZMQ_PRIVATE zframe_t *
    zgossip_msg_routing_id (zgossip_msg_t *self);

//  *** Draft method, defined for internal use only ***
//  Set the message routing id from a frame
CZMQ_PRIVATE void
    zgossip_msg_set_routing_id (zgossip_msg_t *self, zframe_t *routing_id);

//  *** Draft method, defined for internal use only ***
//  Get the zgossip_msg message id
CZMQ_PRIVATE int
    zgossip_msg_id (zgossip_msg_t *self);

//  *** Draft method, defined for internal use only ***
//  Set the zgossip_msg message id
CZMQ_PRIVATE void
    zgossip_msg_set_id (zgossip_msg_t *self, int id);

//  *** Draft method, defined for internal use only ***
//  Get the zgossip_msg message id as printable text
CZMQ_PRIVATE const char *
    zgossip_msg_command (zgossip_msg_t *self);

//  *** Draft method, defined for internal use only ***
//  Get the key field
CZMQ_PRIVATE const char *
    zgossip_msg_key (zgossip_msg_t *self);

//  *** Draft method, defined for internal use only ***
//  Set the key field
CZMQ_PRIVATE void
    zgossip_msg_set_key (zgossip_msg_t *self, const char *key);

//  *** Draft method, defined for internal use only ***
//  Get the value field
CZMQ_PRIVATE const char *
    zgossip_msg_value (zgossip_msg_t *self);

//  *** Draft method, defined for internal use only ***
//  Set the value field
CZMQ_PRIVATE void
    zgossip_msg_set_value (zgossip_msg_t *self, const char *value);

//  *** Draft method, defined for internal use only ***
//  Get the ttl field
CZMQ_PRIVATE uint32_t
    zgossip_msg_ttl (zgossip_msg_t *self);

//  *** Draft method, defined for internal use only ***
//  Set the ttl field
CZMQ_PRIVATE void
    zgossip_msg_set_ttl (zgossip_msg_t *self, uint32_t ttl);

//  *** Draft method, defined for internal use only ***
//  Self test of this class.
CZMQ_PRIVATE void
    zgossip_msg_test (bool verbose);

//  Self test for private classes
CZMQ_PRIVATE void
    czmq_private_selftest (bool verbose);

#endif // CZMQ_BUILD_DRAFT_API

#endif
