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

/*
@header
    The zrex class provides a simple API for regular expressions, wrapping
    Alberto Demichelis's T-Rex library from http://tiny-rex.sourceforge.net/.
@discuss
    The underlying TRex class implements the following expressions:

    \   Quote the next metacharacter
    ^   Match the beginning of the string
    .   Match any character
    $   Match the end of the string
    |   Alternation
    ()  Grouping (creates a capture)
    []  Character class

    ==GREEDY CLOSURES==
    *      Match 0 or more times
    +      Match 1 or more times
    ?      Match 1 or 0 times
    {n}    Match exactly n times
    {n,}   Match at least n times
    {n,m}  Match at least n but not more than m times

    ==ESCAPE CHARACTERS==
    \t      tab                   (HT, TAB)
    \n      newline               (LF, NL)
    \r      return                (CR)
    \f      form feed             (FF)

    ==PREDEFINED CLASSES==
    \l      lowercase next char
    \u      uppercase next char
    \a      letters
    \w      alphanumeric [0-9a-zA-Z]
    \s      space
    \d      decimal digits
    \x      hexadecimal digits
    \c      control characters
    \p      punctuation
    \b      word boundary
    \A      non letters
    \W      non alphanumeric
    \S      non space
    \D      non decimal digits
    \X      non hexadecimal digits
    \C      non control characters
    \P      non punctuation
    \B      non word boundary
@end
*/

#include "../include/czmq.h"

#undef _UNICODE
#define TREX_API    //  Empty, trex not exported to API
#include "../foreign/trex/trex.h"
#include "../foreign/trex/trex.c"

//  Structure of our class

struct _zrex_t {
    TRex *trex;                 //  Compiled regular expression
    const char *strerror;       //  Error message if any
};


//  --------------------------------------------------------------------------
//  Constructor; compile a new regular expression. If the expression is not
//  valid, will still return a zrex_t object but all methods on this will
//  fail, except zrex_strerror () and zrex_valid ().

zrex_t *
zrex_new (const char *expression)
{
    zrex_t *self = (zrex_t *) zmalloc (sizeof (zrex_t));
    assert (self);
    self->strerror = "No error";
    //  Trex cannot handle an empty pattern, which doesn't inspire huge
    //  confidence but apart from this, seems to be working...
    if (*expression)
        self->trex = trex_compile (expression, &self->strerror);
    else
        self->strerror = "Missing pattern";
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zrex_destroy (zrex_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zrex_t *self = *self_p;
        trex_free (self->trex);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return true if the expression was valid and compiled without errors.

bool
zrex_valid (zrex_t *self)
{
    assert (self);
    return self->trex != NULL;
}


//  --------------------------------------------------------------------------
//  Return the error message generated during compilation of the expression.

const char *
zrex_strerror (zrex_t *self)
{
    assert (self);
    return self->strerror;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zrex_test (bool verbose)
{
    printf (" * zrex: ");

    zrex_t *rex = zrex_new ("");
    assert (rex);
    assert (!zrex_valid (rex));
    if (verbose)
        puts (zrex_strerror (rex));
    zrex_destroy (&rex);

    printf ("OK\n");
    return 0;
}
