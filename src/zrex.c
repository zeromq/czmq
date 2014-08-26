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
//  set a pattern, you can call zrex_matches() to test it against text.

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
printf ("a: NUMCAPS=%d\n", self->slre.num_caps); ///
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
//  Returns true if the text matches the previously compiled expression.
//  Use this method to compare one expression against many strings.

bool
zrex_matches (zrex_t *self, const char *text)
{
    assert (self);
    assert (text);

    //  Free any previously-allocated hits
    uint index;
    for (index = 0; index < self->hits; index++) {
        free (self->hit [index]);
        self->hit [index] = NULL;
    }
    bool matches = slre_match (&self->slre, text, strlen (text), self->caps);
    if (matches) {
printf ("b: NUMCAPS=%d\n", self->slre.num_caps); ///
        //  Count number of captures plus whole string
        self->hits = self->slre.num_caps + 1;
        puts ("RESET 2");
        if (self->hits > MAX_HITS)
            self->hits = MAX_HITS;
    }
    else {
        puts ("RESET 1");
        self->hits = 0;
    }

    return matches;
}


//  --------------------------------------------------------------------------
//  Returns true if the text matches the supplied expression. Use this
//  method to compare one string against several expressions.

bool
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

    //  zrex_matches takes care of the rest for us
    if (self->valid)
        return zrex_matches (self, text);
    else
        return false;
}


//  --------------------------------------------------------------------------
//  Returns number of hits from last zrex_matches or zrex_eq. If the text
//  matched, returns 1 plus the number of capture groups. If the text did
//  not match, returns zero. To retrieve individual capture groups, call
//  zrex_hit ().

int
zrex_hits (zrex_t *self)
{
    assert (self);
printf ("c: NUMCAPS=%d\n", self->slre.num_caps); ///
    return self->hits;
}


//  --------------------------------------------------------------------------
//  Returns the Nth capture group from the last expression match, where
//  N is 0 to the value returned by zrex_hits(). Capture group 0 is the
//  whole matching string. Sequence 1 is the first capture group, if any,
//  and so on.

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
    bool matches = zrex_matches (rex, "123-456-789");
    assert (matches);
    assert (zrex_hits (rex) == 1);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (zrex_hit (rex, 1) == NULL);
    zrex_destroy (&rex);

    //  Here we pick out hits using capture groups
    rex = zrex_new ("(\\d+)-(\\d+)-(\\d+)");
    assert (rex);
    assert (zrex_valid (rex));
    matches = zrex_matches (rex, "123-456-ABC");
    assert (!matches);
    matches = zrex_matches (rex, "123-456-789");
    assert (matches);
    printf ("HITS=%d\n", zrex_hits (rex));
    assert (zrex_hits (rex) == 4);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (streq (zrex_hit (rex, 1), "123"));
    assert (streq (zrex_hit (rex, 2), "456"));
    assert (streq (zrex_hit (rex, 3), "789"));
    zrex_destroy (&rex);

    //  This shows the pattern of matching one line against many
    //  patterns and then handling the case when it hits
    rex = zrex_new (NULL);      //  No initial pattern
    char *input = "Mechanism: CURVE";
    matches = zrex_eq (rex, input, "Version: (.+)");
    assert (!matches);
    assert (zrex_hits (rex) == 0);
    matches = zrex_eq (rex, input, "Mechanism: (.+)");
    assert (matches);
    assert (zrex_hits (rex) == 2);
    assert (streq (zrex_hit (rex, 1), "CURVE"));
    zrex_destroy (&rex);
    printf ("OK\n");
}
