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
    ()  Grouping (captures a 'hit')
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
    \a      alpha [a-zA-Z]
    \w      alphanumeric [0-9a-zA-Z]
    \s      space characters
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

#undef _UNICODE
#define TREX_API    //  Empty, trex not exported to API
#include "../foreign/trex/trex.h"
#include "../foreign/trex/trex.c"
#include "../include/czmq.h"

#define MAX_HITS 100            //  Should be enough for anyone :)

//  Structure of our class

struct _zrex_t {
    TRex *trex;                 //  Compiled regular expression
    const char *strerror;       //  Error message if any
    uint hits;                  //  Number of hits matched
    char *hit [MAX_HITS];       //  Captured hits
};


//  --------------------------------------------------------------------------
//  Constructor. Optionally, sets an expression against which we can match
//  text and capture hits. If there is an error in the expression, reports
//  zrex_valid() as false and provides the error in zrex_strerror(). If you
//  set a pattern, you can call zrex_hits() to test it against text.

zrex_t *
zrex_new (const char *expression)
{
    zrex_t *self = (zrex_t *) zmalloc (sizeof (zrex_t));
    assert (self);
    self->strerror = "No error";
    //  Trex cannot handle an empty pattern, which doesn't inspire huge
    //  confidence but apart from this, seems to be working...
    if (expression) {
        if (*expression)
            self->trex = trex_compile (expression, &self->strerror);
        else
            self->strerror = "Missing pattern";
    }
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
        uint index;
        for (index = 0; index < self->hits; index++)
            free (self->hit [index]);
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
//  Matches the text against a previously set expression, and reports the
//  number of hits (aka "capture groups" in e.g. Perl). If the text does
//  not match, returns 0. If it matches, returns 1 or greater, depending on
//  how many "(...)" groups the expression has. An expression with one group
//  will produce 2 hits, one for the whole expression and one for the group.
//  To retrieve the individual hits, call zrex_hit ().

int
zrex_hits (zrex_t *self, const char *text)
{
    assert (self);
    assert (text);
    assert (self->trex);

    uint index;
    for (index = 0; index < self->hits; index++) {
        free (self->hit [index]);
        self->hit [index] = NULL;
    }
    bool matched = trex_match (self->trex, text) == TRex_True;
    if (matched) {
        //  Get number of hits, setting a sane limit
        self->hits = trex_getsubexpcount (self->trex);
        if (self->hits > MAX_HITS)
            self->hits = MAX_HITS;
    }
    else
        self->hits = 0;

    return self->hits;
}


//  --------------------------------------------------------------------------
//  Matches the text against a new expression, and reports the number of
//  hits. If the text does not match, returns 0. If it matches, returns 1 or
//  greater, depending on how many "(...)" groups the expression has. An
//  expression with one group will produce 2 hits, one for the whole
//  expression and one for the group. To retrieve the individual hits, call
//  zrex_hit ().

int
zrex_eq (zrex_t *self, const char *text, const char *expression)
{
    assert (self);
    assert (text);
    assert (expression);

    //  If we had any previous expression, destroy it
    if (self->trex) {
        trex_free (self->trex);
        self->trex = NULL;
    }
    //  Compile the new expression
    if (*expression)
        self->trex = trex_compile (expression, &self->strerror);
    else
        self->strerror = "Missing pattern";

    //  zrex_hits takes care of the rest for us
    return zrex_hits (self, text);

}


//  --------------------------------------------------------------------------
//  Returns the Nth hit captured from the last expression match, where N is
//  0 up to the value returned by zrex_hits() or zrex_eq(). Hit 0 is always
//  the whole matching string. Hit 1 is the first capture group, if any, and
//  so on.

const char *
zrex_hit (zrex_t *self, uint index)
{
    assert (self);
    assert (self->trex);

    if (index < self->hits) {
        //  We collect hits opportunistically to minimize use of the heap for
        //  complex expressions where the caller wants only a few hits.
        if (self->hit [index] == NULL) {
            //  We haven't fetched this hit yet, so grab it now
            TRexMatch match = { 0 };
            trex_getsubexp (self->trex, index, &match);
            self->hit [index] = (char *) malloc (match.len + 1);
            memcpy (self->hit [index], match.begin, match.len);
            self->hit [index][match.len] = 0;
        }
        return self->hit [index];
    }
    else
        return NULL;
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

    //  This shows the pattern of matching many lines to a single pattern
    rex = zrex_new ("[0-9]+\\-[0-9]+\\-[0-9]+");
    assert (rex);
    int hits = zrex_hits (rex, "123-456-789");
    assert (hits == 1);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (zrex_hit (rex, 1) == NULL);
    zrex_destroy (&rex);

    //  Here we pick out hits using capture groups
    rex = zrex_new ("([0-9]+)\\-([0-9]+)\\-([0-9]+)");
    assert (rex);
    assert (zrex_valid (rex));
    hits = zrex_hits (rex, "123-456-ABC");
    assert (hits == 0);
    hits = zrex_hits (rex, "123-456-789");
    assert (hits == 4);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (streq (zrex_hit (rex, 1), "123"));
    assert (streq (zrex_hit (rex, 2), "456"));
    assert (streq (zrex_hit (rex, 3), "789"));
    zrex_destroy (&rex);

    //  This shows the pattern of matching one line against many
    //  patterns and then handling the case when it hits
    rex = zrex_new (NULL);      //  No initial pattern
    char *input = "Mechanism: CURVE";
    if (zrex_eq (rex, input, "Version: (.+)"))
        assert (false);
    else
    if (zrex_eq (rex, input, "Mechanism: (.+)"))
        assert (streq (zrex_hit (rex, 1), "CURVE"));

    zrex_destroy (&rex);
    printf ("OK\n");
    return 0;
}
