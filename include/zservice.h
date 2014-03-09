/*  =========================================================================
    zservice - network service registry and lookup

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
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

#ifndef __ZSERVICE_H_INCLUDED__
#define __ZSERVICE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zservice_t zservice_t;

//  @interface
//  Create a new service instance
CZMQ_EXPORT zservice_t *
    zservice_new (zctx_t *ctx);

//  Destroy a service instance
CZMQ_EXPORT void
    zservice_destroy (zservice_t **self_p);

//  Enable verbose tracing of commands and activity
CZMQ_EXPORT void
    zservice_set_verbose (zservice_t *self, bool verbose);

//  Register a new network service
CZMQ_EXPORT void
    zservice_register (zservice_t *self, const char *name, const char *endpoint);

//  Lookup a network service, return endpoint or NULL if not known
CZMQ_EXPORT char *
    zservice_lookup (zservice_t *self, const char *name);

// Self test of this class
CZMQ_EXPORT void
    zservice_test (bool verbose);

// @end

#ifdef __cplusplus
}
#endif

#endif
