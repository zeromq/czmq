/*  =========================================================================
    zrex - work with regular expressions

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
//  Constructor. Optionally, sets an expression against which we can match
//  text and capture hits. If there is an error in the expression, reports
//  zrex_valid() as false and provides the error in zrex_strerror(). If you
//  set a pattern, you can call zrex_hits() to test it against text.
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

//  Matches the text against a previously set expression, and reports the
//  number of hits (aka "capture groups" in e.g. Perl). If the text does
//  not match, returns 0. If it matches, returns 1 or greater, depending on
//  how many "(...)" groups the expression has. An expression with one group
//  will produce 2 hits, one for the whole expression and one for the group.
//  To retrieve the individual hits, call zrex_hit ().
CZMQ_EXPORT int
    zrex_hits (zrex_t *self, const char *text);

//  Matches the text against a new expression, and reports the number of
//  hits. If the text does not match, returns 0. If it matches, returns 1 or
//  greater, depending on how many "(...)" groups the expression has. An
//  expression with one group will produce 2 hits, one for the whole
//  expression and one for the group. To retrieve the individual hits, call
//  zrex_hit ().
CZMQ_EXPORT int
    zrex_eq (zrex_t *self, const char *text, const char *expression);

//  Returns the Nth sequence captured from the last expression match, where
//  N is 0 to the value returned by zrex_hits() or zrex_eq(). Sequence 0
//  is always the whole matching string. Sequence 1 is the first capture
//  group, if any, and so on.
CZMQ_EXPORT const char *
    zrex_hit (zrex_t *self, uint index);

//  Self test of this class
CZMQ_EXPORT int
    zrex_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
