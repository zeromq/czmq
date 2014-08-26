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
@discuss
@end
*/

#include "../include/czmq.h"
#include "../foreign/slre/slre.h"
#include "../foreign/slre/slre.c"

#define MAX_HITS 100            //  Should be enough for anyone :)

//  Structure of our class

struct _zrex_t {
    struct slre slre;           //  Compiled regular expression
    bool valid;                 //  Is expression valid or not?
    const char *strerror;       //  Error message if any
    uint hits;                  //  Number of hits matched
    char *hit [MAX_HITS];       //  Captured hits
    struct cap caps [MAX_HITS]; //  Position/length for each hit
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
    if (expression) {
        //  Returns 1 on success, 0 on failure
        self->valid = (slre_compile (&self->slre, expression) == 1);
        if (!self->valid)
            self->strerror = self->slre.err_str;
        assert (self->slre.num_caps < MAX_HITS);
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
    return self->valid;
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

    uint index;
    for (index = 0; index < self->hits; index++) {
        free (self->hit [index]);
        self->hit [index] = NULL;
    }
    if (slre_match (&self->slre, text, strlen (text), self->caps)) {
        //  Count number of captures plus whole string
        self->hits = self->slre.num_caps + 1;
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
    //  Compile the new expression
    self->valid = (slre_compile (&self->slre, expression) == 1);
    if (!self->valid)
        self->strerror = self->slre.err_str;
    assert (self->slre.num_caps < MAX_HITS);

    //  zrex_hits takes care of the rest for us
    if (self->valid)
        return zrex_hits (self, text);
    else
        return 0;
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

    if (index < self->hits) {
        //  We collect hits opportunistically to minimize use of the heap for
        //  complex expressions where the caller wants only a few hits.
        if (self->hit [index] == NULL) {
            //  We haven't fetched this hit yet, so grab it now
            int capture_len = self->caps [index].len;
            const char *capture_ptr = self->caps [index].ptr;
            self->hit [index] = (char *) malloc (capture_len + 1);
            memcpy (self->hit [index], capture_ptr, capture_len);
            self->hit [index][capture_len] = 0;
        }
        return self->hit [index];
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Selftest

void
zrex_test (bool verbose)
{
    printf (" * zrex: ");

    //  This shows the pattern of matching many lines to a single pattern
    zrex_t *rex = zrex_new ("\\d+-\\d+-\\d+");
    assert (rex);
    assert (zrex_valid (rex));
    int hits = zrex_hits (rex, "123-456-789");
    assert (hits == 1);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (zrex_hit (rex, 1) == NULL);
    zrex_destroy (&rex);

    //  Here we pick out hits using capture groups
    rex = zrex_new ("(\\d+)-(\\d+)-(\\d+)");
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
}
