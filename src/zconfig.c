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
    text format for configuration files.
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
    zlist_t *comments;          //  Comments if any
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
zconfig_new (char *name, zconfig_t *parent)
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
        zlist_destroy (&self->comments);
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
//  Insert or update configuration key with value; leading slash is optional 
//  and ignored.

void
zconfig_put (zconfig_t *self, char *path, char *value)
{
    if (*path == '/')
        path++;
        
    //  Check length of next path segment
    char *slash = strchr (path, '/');
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
                zconfig_set_value (child, "%s", value);
            return;
        }
        child = child->next;
    }
    //  This segment doesn't exist, create it
    child = zconfig_new (path, self);
    child->name [length] = 0;
    if (slash)                  //  Recurse down further
        zconfig_put (child, slash, value);
    else
        zconfig_set_value (child, "%s", value);
}

    
//  --------------------------------------------------------------------------
//  Set new name for config item; this may be null.

void
zconfig_set_name (zconfig_t *self, char *name)
{
    assert (self);
    free (self->name);
    self->name = name? strdup (name): NULL;
}


//  --------------------------------------------------------------------------
//  Set new value for config item. The new value may be a string, a printf
//  format, or NULL. Note that if string may possibly contain '%', or if it
//  comes from an insecure source, you must use '%s' as the format, followed
//  by the string.

void
zconfig_set_value (zconfig_t *self, char *format, ...)
{
    assert (self);
    free (self->value);
    if (format) {
        va_list argptr;
        va_start (argptr, format);
        self->value = zsys_vprintf (format, argptr);
        va_end (argptr);
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
//  Find a config item along a path; leading slash is optional and ignored.

zconfig_t *
zconfig_locate (zconfig_t *self, char *path)
{
    //  Check length of next path segment
    if (*path == '/')
        path++;
    char *slash = strchr (path, '/');
    int length = strlen (path);
    if (slash)
        length = slash - path;

    //  Find matching name starting at first child of root
    zconfig_t *child = self->child;
    while (child) {
        if (strlen (child->name) == length
        &&  memcmp (child->name, path, length) == 0) {
            if (slash)          //  Look deeper
                return zconfig_locate (child, slash);
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
zconfig_resolve (zconfig_t *self, char *path, char *default_value)
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
//  Dump the config file to stderr for tracing

void
zconfig_dump (zconfig_t *self)
{
    zconfig_execute (self, s_config_save, stderr);
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
zconfig_load (char *filename)
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
        while (length && isspace ((byte) cur_line [length - 1]))
            cur_line [--length] = 0;

        //  Collect indentation level and name, if any
        lineno++;
        //  Handle whole-line comment if present
        if (cur_line [0] == '#') {
            if (!self->comments) {
                self->comments = zlist_new ();
                zlist_autofree (self->comments);
            }
            zlist_append (self->comments, cur_line + 1);
        }
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
//  Add comment to config item before saving to disk. You can add as many
//  comment lines as you like. If you use a null format, all comments are 
//  deleted.

void
zconfig_comment (zconfig_t *self, char *format, ...)
{
    if (format) {
        if (!self->comments) {
            self->comments = zlist_new ();
            zlist_autofree (self->comments);
        }
        va_list argptr;
        va_start (argptr, format);
        char *string = zsys_vprintf (format, argptr);
        va_end (argptr);
        
        zlist_append (self->comments, string);
        free (string);
    }
    else
        zlist_destroy (&self->comments);
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
        fflush (file);
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
    //  Save any comments on the item
    if (self->comments) {
        char *comment = (char *) zlist_first (self->comments);
        while (comment) {
            fprintf (file, "#%s\n", comment);
            comment = (char *) zlist_next (self->comments);
        }
        //  Blank line after comments is nice
        fprintf (file, "\n");
    }
    if (level > 0) {
        if (self->value)
            fprintf (file, "%*s%s = \"%s\"\n", (level - 1) * 4, "",
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
    //  Create temporary directory for test files
#   define TESTDIR ".test_zconfig"
    zsys_dir_create (TESTDIR);
    
    zconfig_t *root = zconfig_new ("root", NULL);
    zconfig_t *section, *item;
    
    section = zconfig_new ("headers", root);
    item = zconfig_new ("email", section);
    zconfig_set_value (item, "some@random.com");
    item = zconfig_new ("name", section);
    zconfig_set_value (item, "Justin Kayce");
    zconfig_put (root, "/curve/secret-key", "Top Secret");
    zconfig_comment (root, "   CURVE certificate");
    zconfig_comment (root, "   -----------------");
    zconfig_save (root, TESTDIR "/test.cfg");
    zconfig_destroy (&root);
    root = zconfig_load (TESTDIR "/test.cfg");
    if (verbose)
        zconfig_save (root, "-");
        
    char *email = zconfig_resolve (root, "/headers/email", NULL);
    assert (email);
    assert (streq (email, "some@random.com"));
    char *passwd = zconfig_resolve (root, "/curve/secret-key", NULL);
    assert (passwd);
    assert (streq (passwd, "Top Secret"));

    zconfig_save (root, TESTDIR "/test.cfg");
    zconfig_destroy (&root);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end

    printf ("OK\n");
}

