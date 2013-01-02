/*  =========================================================================
    zsys - system wrapper

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

#ifndef __ZFL_SYS_H_INCLUDED__
#define __ZFL_SYS_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Return file mode
CZMQ_EXPORT mode_t
    zsys_mode (const char *filename);

//  Create a file path if it doesn't exit
CZMQ_EXPORT void
    zsys_mkdir (const char *pathname);

//  Remove a file path if empty
CZMQ_EXPORT void
    zsys_rmdir (const char *pathname);

//  Delete a file
CZMQ_EXPORT void
    zsys_unlink (const char *filename);

//  Check if file is 'stable'
CZMQ_EXPORT bool
    zsys_stable (const char *filename);

//  Self test of this class
int
    zsys_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
