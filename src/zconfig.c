/*  =========================================================================
    zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.

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
    Lets applications load, work with, and save configuration files.
    This implements rfc.zeromq.org/spec:4/ZPL, which is a simple structured
    text format for configuration files. Note that comments are stripped out
    of configuration files when loading them, so loading and saving a config
    file will remove any comments it contains.
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zconfig_t {
    char *name;                 //  Property name if any
    char *value;                //  Property value, if any
    struct _zconfig_t
        *child,                 //  First child if any
        *next,                  //  Next sibling if any
        *parent;                //  Parent if any
};

//  Local functions for parsing and saving ZPL tokens

static int
    s_collect_level (char **start, int lineno);
static char *
    s_collect_name (char **start, int lineno);
static int
    s_verify_eoln (char *readptr, int lineno);
static char *
    s_collect_value (char **start, int lineno);
static int
    s_config_save (zconfig_t *self, void *arg, int level);


//  --------------------------------------------------------------------------
//  Constructor
//
//  Optionally attach new config to parent config, as first or next child.

zconfig_t *
zconfig_new (const char *name, zconfig_t *parent)
{
    zconfig_t *self = (zconfig_t *) zmalloc (sizeof (zconfig_t));
    zconfig_set_name (self, name);
    if (parent) {
        if (parent->child) {
            //  Attach as last child of parent
            zconfig_t *last = parent->child;
            while (last->next)
                last = last->next;
            last->next = self;
        }
        else
            //  Attach as first child of parent
            parent->child = self;
    }
    self->parent = parent;
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zconfig_destroy (zconfig_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zconfig_t *self = *self_p;

        //  Destroy all children and siblings recursively
        if (self->child)
            zconfig_destroy (&self->child);
        if (self->next)
            zconfig_destroy (&self->next);

        free (self->name);
        free (self->value);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return name of config item

char *
zconfig_name (zconfig_t *self)
{
    assert (self);
    return self->name;
}


//  --------------------------------------------------------------------------
//  Return value of config item

char *
zconfig_value (zconfig_t *self)
{
    assert (self);
    return self->value;
}


//  --------------------------------------------------------------------------
//  Insert or update configuration key with value

void
zconfig_put (zconfig_t *self, const char *path, const char *value)
{
    //  Check length of next path segment
    const char *slash = strchr (path, '/');
    int length = strlen (path);
    if (slash)
        length = slash - path;

    //  Find or create items starting at first child of root
    zconfig_t *child = self->child;
    while (child) {
        if (strlen (child->name) == length
        &&  memcmp (child->name, path, length) == 0) {
            //  This segment exists
            if (slash)          //  Recurse to next level
                zconfig_put (child, slash + 1, value);
            else
                zconfig_set_value (child, value);
            return;
        }
        child = child->next;
    }
    //  This segment doesn't exist, create it
    child = zconfig_new (path, self);
    child->name [length] = 0;
    if (slash)                  //  Recurse down further
        zconfig_put (child, slash + 1, value);
    else
        zconfig_set_value (child, value);
}

    
//  --------------------------------------------------------------------------
//  Set new name for config item. The new name may be a string, a printf
//  format, or NULL.

void
zconfig_set_name (zconfig_t *self, const char *format, ...)
{
    assert (self);
    free (self->name);
    if (format) {
        char formatted [255];
        va_list args;
        va_start (args, format);
        vsnprintf (formatted, 255, format, args);
        va_end (args);
        self->name = strdup (formatted);
    }
    else
        self->name = NULL;
}


//  --------------------------------------------------------------------------
//  Set new value for config item. The new value may be a string, a printf
//  format, or NULL.

void
zconfig_set_value (zconfig_t *self, const char *format, ...)
{
    assert (self);
    free (self->value);
    if (format) {
        char formatted [255];
        va_list args;
        va_start (args, format);
        vsnprintf (formatted, 255, format, args);
        va_end (args);
        self->value = strdup (formatted);
    }
    else
        self->value = NULL;
}


//  --------------------------------------------------------------------------
//  Find our first child, if any

zconfig_t *
zconfig_child (zconfig_t *self)
{
    assert (self);
    return self->child;
}


//  --------------------------------------------------------------------------
//  Find our first sibling, if any

zconfig_t *
zconfig_next (zconfig_t *self)
{
    assert (self);
    return self->next;
}


//  --------------------------------------------------------------------------
//  Find a config item along a path

zconfig_t *
zconfig_locate (zconfig_t *self, const char *path)
{
    //  Check length of next path segment
    const char *slash = strchr (path, '/');
    int length = strlen (path);
    if (slash)
        length = slash - path;

    //  Find matching name starting at first child of root
    zconfig_t *child = self->child;
    while (child) {
        if (strlen (child->name) == length
        &&  memcmp (child->name, path, length) == 0) {
            if (slash)          //  Look deeper
                return zconfig_locate (child, slash + 1);
            else
                return child;
        }
        child = child->next;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Resolve a config path into a string value

char *
zconfig_resolve (zconfig_t *self, const char *path, const char *default_value)
{
    zconfig_t *item = zconfig_locate (self, path);
    if (item)
        return zconfig_value (item);
    else
        return (char *) default_value;
}


//  --------------------------------------------------------------------------
//  Finds the latest node at the specified depth, where 0 is the root. If no
//  such node exists, returns NULL.

zconfig_t *
zconfig_at_depth (zconfig_t *self, int level)
{
    while (level > 0) {
        if (self->child) {
            self = self->child;
            while (self->next)
                self = self->next;
            level--;
        }
        else
            return NULL;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Execute a callback for each config item in the tree

static int
s_config_execute (zconfig_t *self, zconfig_fct handler, void *arg, int level)
{
    assert (self);
    int rc = handler (self, arg, level);

    //  Process all children in one go, as a list
    zconfig_t *child = self->child;
    while (child && !rc) {
        rc = s_config_execute (child, handler, arg, level + 1);
        if (rc == -1)
            break;              //  -1 from callback means end execution
        child = child->next;
    }
    return rc;
}

int
zconfig_execute (zconfig_t *self, zconfig_fct handler, void *arg)
{
    //  Execute top level config at level zero
    assert (self);
    return s_config_execute (self, handler, arg, 0);
}


//  --------------------------------------------------------------------------
//  Dump the config file to stdout for tracing
void
zconfig_dump (zconfig_t *self)
{
}


//  --------------------------------------------------------------------------
//  Load a config item tree from a specified ZPL text file
//
//  Here is an example ZPL stream and corresponding config structure:
//
//  context
//      iothreads = 1
//      verbose = 1      #   Ask for a trace
//  main
//      type = zqueue    #  ZMQ_DEVICE type
//      frontend
//          option
//              hwm = 1000
//              swap = 25000000     #  25MB
//          bind = 'inproc://addr1'
//          bind = 'ipc://addr2'
//      backend
//          bind = inproc://addr3
//
//  root                    Down = child
//    |                     Across = next
//    v
//  context-->main
//    |         |
//    |         v
//    |       type=queue-->frontend-->backend
//    |                      |          |
//    |                      |          v
//    |                      |        bind=inproc://addr3
//    |                      v
//    |                    option-->bind=inproc://addr1-->bind=ipc://addr2
//    |                      |
//    |                      v
//    |                    hwm=1000-->swap=25000000
//    v
//  iothreads=1-->verbose=false

zconfig_t *
zconfig_load (const char *filename)
{
    FILE *file = fopen (filename, "r");
    if (!file)
        return NULL;            //  File not found, or unreadable

    //  Parse the file line by line
    zconfig_t *self = zconfig_new ("root", NULL);
    char cur_line [1024];
    bool valid = true;
    int lineno = 0;
    
    while (fgets (cur_line, 1024, file)) {
        //  Trim line
        int length = strlen (cur_line);
        while (isspace ((byte) cur_line [length - 1]))
            cur_line [--length] = 0;

        //  Collect indentation level and name, if any
        lineno++;
        char *scanner = cur_line;
        int level = s_collect_level (&scanner, lineno);
        if (level == -1) {
            valid = false;
            break;
        }
        char *name = s_collect_name (&scanner, lineno);
        if (name == NULL) {
            valid = false;
            break;
        }
        //  If name is not empty, collect property value
        if (*name) {
            char *value = s_collect_value (&scanner, lineno);
            if (value == NULL)
                valid = false;
            else {
                //  Navigate to parent for this element
                zconfig_t *parent = zconfig_at_depth (self, level);
                if (parent) {
                    zconfig_t *item = zconfig_new (name, parent);
                    item->value = value;
                }
                else {
                    fprintf (stderr, "E: (%d) indentation error\n", lineno);
                    free (value);
                    valid = false;
                }
            }
        }
        else
        if (s_verify_eoln (scanner, lineno))
            valid = false;
        free (name);
        if (!valid)
            break;
    }
    //  Either the whole ZPL file is valid or none of it is
    if (!valid)
        zconfig_destroy (&self);
    fclose (file);
    return self;
}


//  Count and verify indentation level, -1 means a syntax error

static int
s_collect_level (char **start, int lineno)
{
    char *readptr = *start;
    while (*readptr == ' ')
        readptr++;
    int level = (readptr - *start) / 4;
    if (level * 4 != readptr - *start) {
        fprintf (stderr, "E: (%d) indent 4 spaces at once\n", lineno);
        level = -1;
    }
    *start = readptr;
    return level;
}

//  Collect property name

static bool
s_is_namechar (char thischar) 
{
    return (isalnum (thischar) 
         || thischar == '$'
         || thischar == '-'
         || thischar == '_'
         || thischar == '@'
         || thischar == '.'
         || thischar == '&'
         || thischar == '+'
         || thischar == '/');
}

static char *
s_collect_name (char **start, int lineno)
{
    char *readptr = *start;
    while (s_is_namechar ((char) **start))
        (*start)++;

    size_t length = *start - readptr;
    char *name = (char *) zmalloc (length + 1);
    memcpy (name, readptr, length);
    name [length] = 0;
    
    if (length > 0
    && (name [0] == '/' || name [length - 1] == '/')) {
        fprintf (stderr, "E: (%d) '/' not valid at name start or end\n", lineno);
        free (name);
    }
    return name;
}

//  Check there's no junk after value on line, returns 0 if OK else -1.

static int
s_verify_eoln (char *readptr, int lineno)
{
    while (*readptr) {
        if (isspace ((byte) *readptr))
            readptr++;
        else
        if (*readptr == '#')
            break;
        else {
            fprintf (stderr, "E: (%d) invalid syntax '%s'\n",
                lineno, readptr);
            return -1;
            break;
        }
    }
    return 0;
}

//  Return value for name, or "" - if syntax error, returns NULL.

static char *
s_collect_value (char **start, int lineno)
{
    char *value = NULL;
    char *readptr = *start;
    int rc = 0;

    while (isspace ((byte) *readptr))
        readptr++;

    if (*readptr == '=') {
        readptr++;
        while (isspace ((byte) *readptr))
            readptr++;

        //  If value starts with quote or apost, collect it
        if (*readptr == '"' || *readptr == '\'') {
            char *endquote = strchr (readptr + 1, *readptr);
            if (endquote) {
                size_t value_length = endquote - readptr - 1;
                value = (char *) zmalloc (value_length + 1);
                memcpy (value, readptr + 1, value_length);
                value [value_length] = 0;
                rc = s_verify_eoln (endquote + 1, lineno);
            }
            else {
                fprintf (stderr, "E: (%d) missing %c\n", lineno, *readptr);
                rc = -1;
            }
        }
        else {
            //  Collect unquoted value up to comment
            char *comment = strchr (readptr, '#');
            if (comment) {
                while (isspace ((byte) comment [-1]))
                    comment--;
                *comment = 0;
            }
            value = strdup (readptr);
        }
    }
    else {
        value = strdup ("");
        rc = s_verify_eoln (readptr, lineno);
    }
    //  If we had an error, drop value and return NULL
    if (rc)
        free (value);
    return value;
}


//  --------------------------------------------------------------------------
//  Save a config item tree to a specified ZPL text file, where a filename
//  "-" means dump to standard output.

int
zconfig_save (zconfig_t *self, char *filename)
{
    assert (self);

    int rc = 0;
    if (streq (filename, "-")) {
        //  "-" means write to stdout
        rc = zconfig_execute (self, s_config_save, stdout);
    }
    else {
        FILE *file;
        file = fopen (filename, "w");
        if (file)
            rc = zconfig_execute (self, s_config_save, file);
        else
            rc = -1;          //  File not writeable
        fclose (file);
    }
    return rc;
}

//  Save an item

static int
s_config_save (zconfig_t *self, void *arg, int level)
{
    assert (self);
    assert (arg);

    FILE *file = (FILE *) arg;
    if (level > 0) {
        if (self->value)
            fprintf (file, "%*s%s = %s\n", (level - 1) * 4, "",
                self->name? self->name: "(Unnamed)", self->value);
        else
            fprintf (file, "%*s%s\n", (level - 1) * 4, "",
                self->name? self->name: "(Unnamed)");
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Self test of this class

void
zconfig_test (bool verbose)
{
    printf (" * zconfig: ");

    //  @selftest
    //  We create a config of this structure:
    //
    //  root
    //      type = zqueue
    //      frontend
    //          option
    //              swap = 25000000     #  25MB
    //              subscribe = #2
    //              hwm = 1000
    //          bind = tcp://*:5555
    //      backend
    //          bind = tcp://*:5556
    //
    zconfig_t
        *root,
        *type,
        *frontend,
        *option,
        *hwm,
        *swap,
        *subscribe,
        *bind,
        *backend;

    //  Left is first child, next is next sibling
    root     = zconfig_new ("root", NULL);
    type     = zconfig_new ("type", root);
    zconfig_set_value (type, "zqueue");
    frontend = zconfig_new ("frontend", root);
    option   = zconfig_new ("option", frontend);
    swap     = zconfig_new ("swap", option);
    zconfig_set_value (swap, "25000000");
    subscribe = zconfig_new ("subscribe", option);
    zconfig_set_value (subscribe, "#%d", 2);
    hwm      = zconfig_new ("hwm", option);
    zconfig_set_value (hwm, "1000");
    bind     = zconfig_new ("bind", frontend);
    zconfig_set_value (bind, "tcp://*:5555");
    backend  = zconfig_new ("backend", root);
    bind     = zconfig_new ("bind", backend);
    zconfig_set_value (bind, "tcp://*:5556");

    assert (atoi (zconfig_resolve (root, "frontend/option/hwm", "0")) == 1000);
    assert (streq (zconfig_resolve (root, "backend/bind", ""), "tcp://*:5556"));

    zconfig_put (root, "frontend/option/hwm", "500");
    assert (atoi (zconfig_resolve (root, "frontend/option/hwm", "0")) == 500);
    zconfig_put (root, "frontend/option/lwm", "200");
    assert (atoi (zconfig_resolve (root, "frontend/option/lwm", "0")) == 200);
    
    zconfig_destroy (&root);
    assert (root == NULL);

    //  Test loading from a ZPL file
    zconfig_t *config = zconfig_load ("selftest.cfg");
    assert (config);

    //  Destructor should be safe to call twice
    zconfig_destroy (&config);
    zconfig_destroy (&config);
    assert (config == NULL);
    //  @end

    printf ("OK\n");
}

