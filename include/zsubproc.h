/*  =========================================================================
    zsubproc - class description

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZSUBPROC_H_INCLUDED
#define ZSUBPROC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new zsubproc
CZMQ_EXPORT zsubproc_t *
    zsubproc_new (void);

//  Destroy the zsubproc
CZMQ_EXPORT void
    zsubproc_destroy (zsubproc_t **self_p);

//  Self test of this class
CZMQ_EXPORT void
    zsubproc_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
