/*  =========================================================================
    zargs - Platform independent command line argument parsing helpers

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
    zargs - Platform independent command line argument parsing helpers

    Platform independent command line argument parsing helpers

    There are two kind of elements provided by this class
    foo --named-parameter --parameter with_value positional arguments -a gain-parameter
    zargs keeps poision only for arguments, parameters are to be accessed like hash.

    It DOES:
    * provide easy to use CLASS compatible API for accessing argv
    * is platform independent
    * provide getopt_long style -- argument, which delimits parameters from arguments
    * makes parameters position independent

    It does NOT
    * change argv
    * provide a "declarative" way to define command line interface

    In future it SHALL
    * hide several formats of command line to one (-Idir, --include=dir,
      --include dir are the same from API pov)
@discuss
@end
*/

#include "czmq_classes.h"

//  Structure of our class
static char *ZARG_PARAM_EMPTY = "";

struct _zargs_t {
    char *progname;	 // program name aka argv [0]
    zlist_t *arguments;  // positional arguments
    zhash_t *parameters; // --named parameters
};


//  --------------------------------------------------------------------------
//  Create a new zargs

zargs_t *
zargs_new (int argc, char **argv)
{
    assert (argc > 0);
    assert (argv);
    zargs_t *self = (zargs_t *) zmalloc (sizeof (zargs_t));
    assert (self);
    //  Initialize class properties here
    self->progname = argv [0];
    assert (self->progname);
    self->arguments = zlist_new ();
    assert (self->arguments);
    self->parameters = zhash_new ();
    assert (self->parameters);

    if (argc == 1)
        return self;

    int idx = 1;
    bool params_only = false;
    while (argv [idx]) {
        if (params_only || argv [idx][0] != '-')
            zlist_append (self->arguments, argv [idx]);
        else {
	    if (streq (argv [idx], "--")) {
	        params_only = true;
            idx ++;
            continue;
	    }
	    else
            if (argv [idx+1] && argv [idx+1][0] != '-') {
                zhash_insert (self->parameters, argv [idx], argv [idx+1]);
                idx ++;
            }
            else {
                zhash_insert (self->parameters, argv [idx], ZARG_PARAM_EMPTY);
            }
        }
        idx ++;
    }

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the zargs

void
zargs_destroy (zargs_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zargs_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        zlist_destroy (&self->arguments);
        zhash_destroy (&self->parameters);
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Return the program name (argv[0]) 

const char *
zargs_progname (zargs_t *self) {
    assert (self);
    return self->progname;
}

//  --------------------------------------------------------------------------
//  Return the number of command line arguments

size_t
zargs_arguments (zargs_t *self) {
    assert (self);
    return zlist_size (self->arguments);
}

//  --------------------------------------------------------------------------
//  Return first command line argument

const char *
zargs_first (zargs_t *self) {
    assert (self);
    return (const char*) zlist_first (self->arguments);
}

//  --------------------------------------------------------------------------
//  Return next command line argument

const char *
zargs_next (zargs_t *self) {
    assert (self);
    return (const char*) zlist_next (self->arguments);
}

//  --------------------------------------------------------------------------
//  Return first command line parameter value

const char *
zargs_param_first (zargs_t *self) {
    assert (self);
    return (const char*) zhash_first (self->parameters);
}

//  --------------------------------------------------------------------------
//  Return next command line parameter value

const char *
zargs_param_next (zargs_t *self) {
    assert (self);
    return (const char*) zhash_next (self->parameters);
}

//  --------------------------------------------------------------------------
//  Return current command line parameter name

const char *
zargs_param_name (zargs_t *self) {
    assert (self);
    return (const char*) zhash_cursor (self->parameters);
}

//  --------------------------------------------------------------------------
//  Return current parameter name, or NULL if there are no named parameters.
const char *
zargs_get (zargs_t *self, const char *name) {
    assert (self);
    assert (name);
    const char *ret = (const char*) zhash_lookup (self->parameters, name);
    return ret != ZARG_PARAM_EMPTY ? ret : NULL;
}

//  --------------------------------------------------------------------------
//  Return value of one of parameter(s) or NULL is it has no value (or was not specified)
CZMQ_EXPORT const char *
    zargs_getx (zargs_t *self, const char *name, ...)
{
    assert (self);
    assert (name);
    va_list args;
    va_start (args, name);
    while (name) {
        const char *ret = zargs_get (self, name);
        if (ret)
            return ret;
        name = va_arg (args, const char*);
    }
    va_end (args);
    return NULL;
}

//  --------------------------------------------------------------------------
//  Returns true if named parameter was specified on command line
bool
zargs_has (zargs_t *self, const char *name) {
    assert (self);
    assert (name);
    const char *ret = (const char*) zhash_lookup (self->parameters, name);
    return ret != NULL;
}

//  --------------------------------------------------------------------------
//  Returns true if named parameter(s) was specified on command line
bool
zargs_hasx (zargs_t *self, const char *name, ...) {
    assert (self);
    va_list args;
    va_start (args, name);
    while (name) {
        bool ret = zargs_has (self, name);
        if (ret)
            return true;
        name = va_arg (args, const char *);
    }
    va_end (args);
    return NULL;
}


//  --------------------------------------------------------------------------
//  Print the zargs instance

void
zargs_print (zargs_t *self) {
    assert (self);
    fprintf (stderr, "%s ", self->progname);
    for (const char *pvalue = zargs_param_first (self);
                    pvalue != NULL;
                    pvalue = zargs_param_next (self)) {
        const char *pname = zargs_param_name (self);
        if (pvalue == ZARG_PARAM_EMPTY)
            fprintf (stderr, "%s : None ", pname);
        else
            fprintf (stderr, "%s : %s ", pname, pvalue);
	fprintf (stderr, ", ");
    }
    for (const char *arg = zargs_first (self);
                     arg != NULL;
                     arg = zargs_next (self)) {
        fprintf (stderr, "%s ", arg);
    }
    fputs ("", stderr);
}
//  --------------------------------------------------------------------------
//  Self test of this class

void
zargs_test (bool verbose)
{
    zsys_init ();
    printf (" * zargs: ");

    //  @selftest
    //  Simple create/destroy test
    
    char *argv1[] = {"progname", "--named1", "-n1", "val1", "positional1", "--with", "value", "--with2=value2", "-W3value3", "--", "--thisis", "considered", "positional", NULL};

    zargs_t *self = zargs_new (13, argv1);
    assert (self);

    assert (streq (zargs_progname (self), "progname"));
    assert (streq (zargs_first (self), "positional1"));
    assert (streq (zargs_next (self), "--thisis"));
    assert (streq (zargs_next (self), "considered"));
    assert (streq (zargs_next (self), "positional"));
    assert (!zargs_next (self));

    assert (zargs_has (self, "--named1"));
    assert (zargs_has (self, "-n1"));
    assert (!zargs_has (self, "--not at all"));

    assert (!(zargs_get (self, "--named1")));
    assert (streq (zargs_get (self, "-n1"), "val1"));

    // common usages - test for -h/--help
    bool has_help = zargs_hasx (self, "--help", "-h", NULL);
    assert (!has_help);

    zargs_destroy (&self);
    //  @end
    printf ("OK\n");
}
