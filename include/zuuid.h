/*  =========================================================================
    zuuid - UUID support class

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZUUID_H_INCLUDED__
#define __ZUUID_H_INCLUDED__

#define ZUUID_LEN    16

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zuuid_t zuuid_t;

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

#ifdef __cplusplus
}
#endif

#endif
