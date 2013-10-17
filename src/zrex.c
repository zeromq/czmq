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
    ()  Grouping (creates a subsequence)
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

#undef _UNICODE
#define TREX_API    //  Empty, trex not exported to API
#include "../foreign/trex/trex.h"
#include "../foreign/trex/trex.c"
#include "../include/czmq.h"

#define MAX_SEQ 100             //  Should be enough for anyone :)

//  Structure of our class

struct _zrex_t {
    TRex *trex;                 //  Compiled regular expression
    const char *strerror;       //  Error message if any
    bool matches;               //  True if last match succeeded
    uint count;                 //  Number of sequences
    char *sequence [MAX_SEQ];   //  Captured sequences
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
        int index;
        for (index = 0; index < self->count; index++)
            free (self->sequence [index]);
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

// Work-around for Windows MS libc not having strndup():
char* my_strndup(const char* src, size_t length) {
#if defined(__WINDOWS__)
    if (!src)
        return "";
    length = strlen(src) > length ? length: strlen(src);
    char* result = (char*) malloc((length + 1) * sizeof(char));
    if (!result)
        return NULL;
    strncpy(result, src, length);
    result[length] = '\0';
    return result;
#else //defined(__WINDOWS__)
    return strndup(src, length);
#endif //defined(__WINDOWS__)
}

//  --------------------------------------------------------------------------
//  Return true if the expression matches a provided string. If true, you
//  can access the matched sequences using zrex_sequence ().

bool
zrex_matches (zrex_t *self, const char *text)
{
    int index;
    for (index = 0; index < self->count; index++)
        free (self->sequence [index]);

    self->matches = trex_match (self->trex, text);
    if (self->matches) {
        self->count = trex_getsubexpcount (self->trex);
        for (index = 0; index < self->count; index++) {
            TRexMatch match = { 0 };
            trex_getsubexp (self->trex, index, &match);
            self->sequence [index] = my_strndup (match.begin, match.len);
        }
    }
    else
        self->count = 0;
    return self->matches;
}


//  --------------------------------------------------------------------------
//  Returns a string holding the sequence at the specified index. If there
//  was no sequence at the specified index, returns NULL. Sequence 0 is the
//  whole matching sequence; sequence 1 is the first subsequence.

const char *
zrex_sequence (zrex_t *self, uint index)
{
    assert (self);
    if (!self->matches || index > MAX_SEQ)
        return NULL;
    else
        return self->sequence [index];
}


//  --------------------------------------------------------------------------
//  Return number of matched sequences, which is 1 or more if the string
//  matched, and zero otherwise.

int
zrex_count (zrex_t *self)
{
    assert (self);
    return self->count;
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

    rex = zrex_new ("([0-9]+)\\-([0-9]+)\\-([0-9]+)");
    assert (rex);
    assert (zrex_valid (rex));
    bool matches = zrex_matches (rex, "123-456-ABC");
    assert (!matches);
    matches = zrex_matches (rex, "123-456-789");
    assert (matches);
    assert (zrex_count (rex) == 4);
    assert (streq (zrex_sequence (rex, 0), "123-456-789"));
    assert (streq (zrex_sequence (rex, 1), "123"));
    assert (streq (zrex_sequence (rex, 2), "456"));
    assert (streq (zrex_sequence (rex, 3), "789"));
    zrex_destroy (&rex);

    rex = zrex_new ("[0-9]+\\-[0-9]+\\-[0-9]+");
    assert (rex);
    matches = zrex_matches (rex, "123-456-789");
    assert (matches);
    assert (zrex_count (rex) == 1);
    assert (streq (zrex_sequence (rex, 0), "123-456-789"));
    assert (zrex_sequence (rex, 1) == NULL);
    zrex_destroy (&rex);

    printf ("OK\n");
    return 0;
}
