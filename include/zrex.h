/*  =========================================================================
    zrex - work with regular expressions

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

#ifndef __ZREX_H_INCLUDED__
#define __ZREX_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zrex_t zrex_t;

//  @interface
//  Constructor; compile a new regular expression. If the expression is not
//  valid, will still return a zrex_t object but all methods on this will
//  fail, except zrex_strerror () and zrex_valid ().
CZMQ_EXPORT zrex_t *
    zrex_new (const char *expression);

//  Destructor
CZMQ_EXPORT void
    zrex_destroy (zrex_t **self_p);

//  Return true if the expression was valid and compiled without errors.
CZMQ_EXPORT bool
    zrex_valid (zrex_t *self);

//  Return the error message generated during compilation of the expression.
CZMQ_EXPORT const char *
    zrex_strerror (zrex_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zrex_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
