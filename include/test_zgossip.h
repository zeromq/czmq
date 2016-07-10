/*  =========================================================================
    test_zgossip - 

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __TEST_ZGOSSIP_H_INCLUDED__
#define __TEST_ZGOSSIP_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif


//  @interface
//  Create a new test_zgossip
CZMQ_EXPORT test_zgossip_t *
    test_zgossip_new (void);

//  Destroy the test_zgossip
CZMQ_EXPORT void
    test_zgossip_destroy (test_zgossip_t **self_p);

//  Print properties of object
CZMQ_EXPORT void
    test_zgossip_print (test_zgossip_t *self);

//  Self test of this class
CZMQ_EXPORT void
    test_zgossip_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
