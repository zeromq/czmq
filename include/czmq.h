/*  =========================================================================
    czmq - an open-source framework for proximity-based P2P apps

    Copyright (c) 1991-2012 iMatix Corporation -- http://www.imatix.com
    Copyright other contributors as noted in the AUTHORS file.         
                                                                       
    This file is part of CZMQ -- See http://czmq.zeromq.org            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef __CZMQ_H_INCLUDED__
#define __CZMQ_H_INCLUDED__

//  These are signatures for handler functions that customize the
//  behavior of CZMQ containers. These are shared between all CZMQ
//  container types.

//  -- destroy an item
typedef void (czmq_destructor) (void **item);
//  -- duplicate an item
typedef void *(czmq_duplicator) (const void *item);
//  - compare two items, for sorting
typedef int (czmq_comparator) (const void *item1, const void *item2);

//  Include the project library file
#include "czmq_library.h"

#endif
