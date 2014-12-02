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
    Wraps a very simple regular expression library (SLRE) as a CZMQ class.
    Supports this syntax:

        ^               Match beginning of a buffer
        $               Match end of a buffer
        ()              Grouping and substring capturing
        [...]           Match any character from set
        [^...]          Match any character but ones from set
        .               Match any character
        \s              Match whitespace
        \S              Match non-whitespace
        \d              Match decimal digit
        \D              Match non decimal digit
        \a              Match alphabetic character
        \A              Match non-alphabetic character
        \w              Match alphanumeric character
        \W              Match non-alphanumeric character
        \r              Match carriage return
        \n              Match newline
        +               Match one or more times (greedy)
        +?              Match one or more times (non-greedy)
        *               Match zero or more times (greedy)
        *?              Match zero or more times (non-greedy)
        ?               Match zero or once
        \xDD            Match byte with hex value 0xDD
        \meta           Match one of the meta character: ^$().[*+?\
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
    uint hit_set_len;           //  Length of hit set
    char *hit_set;              //  Captured hits as single string
    char *hit [MAX_HITS];       //  Pointers into hit_set
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
    if (self) {
        self->strerror = "No error";
        if (expression) {
            //  Returns 1 on success, 0 on failure
            self->valid = (slre_compile (&self->slre, expression) == 1);
            if (!self->valid)
                self->strerror = self->slre.err_str;
            assert (self->slre.num_caps < MAX_HITS);
        }
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
        zstr_free (&self->hit_set);
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
    self->hits = 0;

    bool matches = slre_match (&self->slre, text, strlen (text), self->caps) != 0;
    if (matches) {
        //  Count number of captures plus whole string
        self->hits = self->slre.num_caps + 1;
        if (self->hits > MAX_HITS)
            self->hits = MAX_HITS;

        //  Collect hits and prepare hit array, which is a single block of
        //  memory holding all hits as null-terminated strings
        uint index;
        //  First count total length of hit strings
        uint hit_set_len = 0;
        for (index = 0; index < self->hits; index++)
            hit_set_len += self->caps [index].len + 1;
        if (hit_set_len > self->hit_set_len) {
            zstr_free (&self->hit_set);
            self->hit_set = (char *) zmalloc (hit_set_len);
            self->hit_set_len = hit_set_len;
        }
        // FIXME: no way to return an error
        assert (self->hit_set);

        //  Now prepare hit strings for access by caller
        char *hit_set_ptr = self->hit_set;
        for (index = 0; index < self->hits; index++) {
            memcpy (hit_set_ptr, self->caps [index].ptr, self->caps [index].len);
            self->hit [index] = hit_set_ptr;
            hit_set_ptr += self->caps [index].len + 1;
        }
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
    if (index < self->hits)
        return self->hit [index];
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Fetches hits into string variables provided by caller; this makes for
//  nicer code than accessing hits by index. Caller should not modify nor
//  free the returned values. Returns number of strings returned. This
//  method starts at hit 1, i.e. first capture group, as hit 0 is always
//  the original matched string.

int
zrex_fetch (zrex_t *self, const char **string_p, ...)
{
    assert (self);
    va_list args;
    va_start (args, string_p);
    uint index = 0;
    while (string_p) {
        *string_p = zrex_hit (self, ++index);
        string_p = va_arg (args, const char **);
    }
    va_end (args);
    return index;
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
    assert (zrex_hits (rex) == 4);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (streq (zrex_hit (rex, 1), "123"));
    assert (streq (zrex_hit (rex, 2), "456"));
    assert (streq (zrex_hit (rex, 3), "789"));
    zrex_destroy (&rex);

    //  This shows the pattern of matching one line against many
    //  patterns and then handling the case when it hits
    rex = zrex_new (NULL);      //  No initial pattern
    assert (rex);
    char *input = "Mechanism: CURVE";
    matches = zrex_eq (rex, input, "Version: (.+)");
    assert (!matches);
    assert (zrex_hits (rex) == 0);
    matches = zrex_eq (rex, input, "Mechanism: (.+)");
    assert (matches);
    assert (zrex_hits (rex) == 2);
    const char *mechanism;
    zrex_fetch (rex, &mechanism, NULL);
    assert (streq (zrex_hit (rex, 1), "CURVE"));
    assert (streq (mechanism, "CURVE"));
    zrex_destroy (&rex);
    printf ("OK\n");
}
