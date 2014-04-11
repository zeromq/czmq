/*  =========================================================================
    zuuid - UUID support class

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZUUID_H_INCLUDED__
#define __ZUUID_H_INCLUDED__

#define ZUUID_LEN    16

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zuuid_t zuuid_t;

//  @interface
//  Constructor
CZMQ_EXPORT zuuid_t *
    zuuid_new (void);

//  Destructor
CZMQ_EXPORT void
    zuuid_destroy (zuuid_t **self_p);

//  Return UUID binary data
CZMQ_EXPORT byte *
    zuuid_data (zuuid_t *self);

//  Return UUID binary size
CZMQ_EXPORT size_t
    zuuid_size (zuuid_t *self);

//  Returns UUID as string
CZMQ_EXPORT char *
    zuuid_str (zuuid_t *self);

//  Set UUID to new supplied value 
CZMQ_EXPORT void
    zuuid_set (zuuid_t *self, byte *source);
    
//  Store UUID blob in target array
CZMQ_EXPORT void
    zuuid_export (zuuid_t *self, byte *target);

//  Check if UUID is same as supplied value
CZMQ_EXPORT bool
    zuuid_eq (zuuid_t *self, byte *compare);

//  Check if UUID is different from supplied value
CZMQ_EXPORT bool
    zuuid_neq (zuuid_t *self, byte *compare);

//  Make copy of UUID object
CZMQ_EXPORT zuuid_t *
    zuuid_dup (zuuid_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zuuid_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
