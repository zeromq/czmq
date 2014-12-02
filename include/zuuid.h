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

#define ZUUID_LEN     16
#define ZUUID_STR_LEN (ZUUID_LEN * 2)

#ifdef __cplusplus
extern "C" {
#endif

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

// Return UUID as formatted string in the canonical format 8-4-4-4-12,
// lower case.  The caller should free the freshly allocated string.
// See: http://en.wikipedia.org/wiki/Universally_unique_identifier
CZMQ_EXPORT char *
    zuuid_formatted_str (zuuid_t *self);

//  Set UUID to new supplied ZUUID_LEN-octet value
CZMQ_EXPORT void
    zuuid_set (zuuid_t *self, byte *source);
    
//  Set UUID to new supplied ZUUID_STR_LEN-char string value;
//  return 0 if OK, else returns -1.
CZMQ_EXPORT int
    zuuid_set_str (zuuid_t *self, const char *source);

//  Store UUID blob in target array
CZMQ_EXPORT void
    zuuid_export (zuuid_t *self, byte *target);

//  Check if UUID is same as supplied value
CZMQ_EXPORT bool
    zuuid_eq (zuuid_t *self, byte *compare);

//  Check if UUID is different from supplied value
CZMQ_EXPORT bool
    zuuid_neq (zuuid_t *self, byte *compare);

//  Make copy of UUID object; if uuid is null, or memory was exhausted,
//  returns null.
CZMQ_EXPORT zuuid_t *
    zuuid_dup (zuuid_t *self);

//  Self test of this class
CZMQ_EXPORT void
    zuuid_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
