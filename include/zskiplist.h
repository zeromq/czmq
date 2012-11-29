/*  =========================================================================
    zsklist - a skip list

    -------------------------------------------------------------------------
    Copyright (c) 1991-2012 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/




#ifndef __ZSKIPLIST_H_INCLUDED__
#define __ZSKIPLIST_H_INCLUDED__


#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _zsklnode_t zsklnode_t;

    typedef struct _zsklist_t zsklist_t;

//  Create a new skip list
    CZMQ_EXPORT zsklist_t *zsklist_new (void);

//  Destroy a skip list
    CZMQ_EXPORT void zsklist_destroy (zsklist_t ** zsklist);

//  Add a strictly postive key to the skip list, returns 0 if already present
    CZMQ_EXPORT int zsklist_add (zsklist_t * zsklist, uint64_t key);

//  Remove a key from the skip list, returns 0 if not present
    CZMQ_EXPORT int zsklist_delete (zsklist_t * zsklist, uint64_t key);

//  Search a key, returns the key or zero if not present
    CZMQ_EXPORT uint64_t zsklist_search (zsklist_t * zsklist, uint64_t key);

//  Search a key between 2 limits, returns the zsklist node that contains it
    CZMQ_EXPORT zsklnode_t *zsklist_lsearch (zsklist_t * t, uint64_t key,
                                             zsklnode_t * rlimit,
                                             zsklnode_t * llimit);


//  Selt test of this class
    void zsklist_test (int verbose);



#ifdef __cplusplus
}
#endif





#endif
