/*  =========================================================================
    zsys - system wrapper

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
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

#ifndef __ZSYS_H_INCLUDED__
#define __ZSYS_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Callback for interrupt signal handler
typedef void (zsys_handler_fn) (int signal_value);

//  Set interrupt handler (NULL means external handler)
CZMQ_EXPORT void
    zsys_handler_set (zsys_handler_fn *handler_fn);

//  Reset interrupt handler, call this at exit if needed
CZMQ_EXPORT void
    zsys_handler_reset (void);

//  Self test of this class
CZMQ_EXPORT int
    zsys_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
