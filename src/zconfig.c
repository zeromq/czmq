/*  =========================================================================
    zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.

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
    Lets applications load, work with, and save configuration files.
    This implements rfc.zeromq.org/spec:4/ZPL, which is a simple structured
    text format for configuration files.
@discuss
    Here is an example ZPL stream and corresponding config structure:

        context
            iothreads = 1
            verbose = 1      #   Ask for a trace
        main
            type = zqueue    #  ZMQ_DEVICE type
            frontend
                option
                    hwm = 1000
                    swap = 25000000     #  25MB
                bind = 'inproc://addr1'
                bind = 'ipc://addr2'
            backend
                bind = inproc://addr3

        root                    Down = child
        |                     Across = next
        v
        context-->main
        |         |
        |         v
        |       type=queue-->frontend-->backend
        |                      |          |
        |                      |          v
        |                      |        bind=inproc://addr3
        |                      v
        |                    option-->bind=inproc://addr1-->bind=ipc://addr2
        |                      |
        |                      v
        |                    hwm=1000-->swap=25000000
        v
        iothreads=1-->verbose=false
@end
*/

#include "czmq_classes.h"

//  Structure of our class

struct _zconfig_t {
    char *name;                 //  Property name if any
    char *value;                //  Property value, if any
    struct _zconfig_t
    *child,                     //  First child if any
    *next,                      //  Next sibling if any
    *parent;                    //  Parent if any
    zlist_t *comments;          //  Comments if any
    zfile_t *file;              //  Config file handle
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
static int
s_config_execute (zconfig_t *self, zconfig_fct handler, void *arg, int level);


//  --------------------------------------------------------------------------
//  Constructor
//
//  Optionally attach new config to parent config, as first or next child.

zconfig_t *
zconfig_new (const char *name, zconfig_t *parent)
{
    zconfig_t *self = (zconfig_t *) zmalloc (sizeof (zconfig_t));
    assert (self);

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
        zconfig_destroy (&self->child);
        zconfig_destroy (&self->next);

        //  Destroy other properties and then self
        zlist_destroy (&self->comments);
        zfile_destroy (&self->file);
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
zconfig_put (zconfig_t *self, const char *path, const char *value)
{
    if (*path == '/')
        path++;

    //  Check length of next path segment
    const char *slash = strchr (path, '/');
    size_t length = strlen (path);
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
    assert (child);
    child->name [length] = 0;
    if (slash)                  //  Recurse down further
        zconfig_put (child, slash, value);
    else
        zconfig_set_value (child, "%s", value);
}


//  --------------------------------------------------------------------------
//  Equivalent to zconfig_put, accepting a format specifier and variable
//  argument list, instead of a single string value.

void
zconfig_putf (zconfig_t *self, const char *path, const char *format, ...)
{
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *value = zsys_vprintf (format, argptr);
    va_end (argptr);
    zconfig_put (self, path, value);
    zstr_free (&value);
}


//  --------------------------------------------------------------------------
//  Set new name for config item; this may be null.

void
zconfig_set_name (zconfig_t *self, const char *name)
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
zconfig_set_value (zconfig_t *self, const char *format, ...)
{
    assert (self);
    zstr_free (&self->value);
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
zconfig_locate (zconfig_t *self, const char *path)
{
    assert (self);

    //  Check length of next path segment
    if (*path == '/')
        path++;
    const char *slash = strchr (path, '/');
    size_t length = strlen (path);
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
//  Get value for config item into a string value; leading slash is optional
//  and ignored.

char *
zconfig_get (zconfig_t *self, const char *path, const char *default_value)
{
    assert (self);
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
    assert (self);

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
//  Execute a callback for each config item in the tree; returns zero if
//  successful, else -1.

int
zconfig_execute (zconfig_t *self, zconfig_fct handler, void *arg)
{
    //  Execute top level config at level zero
    assert (self);
    return s_config_execute (self, handler, arg, 0) >= 0? 0: -1;
}


//  Return number of bytes processed if successful, otherwise -1.

static int
s_config_execute (zconfig_t *self, zconfig_fct handler, void *arg, int level)
{
    assert (self);
    int size = handler (self, arg, level);
    if (size == -1)
        return -1; // fail early

    //  Process all children in one go, as a list
    zconfig_t *child = self->child;
    while (child) {
        int rc = s_config_execute (child, handler, arg, level + 1);
        if (rc == -1)
            return -1;
        size += rc;
        child = child->next;
    }
    return size;
}

//  --------------------------------------------------------------------------
//  Load a config tree from a specified ZPL text file; returns a zconfig_t
//  reference for the root, if the file exists and is readable. Returns NULL
//  if the file does not exist.

zconfig_t *
zconfig_load (const char *filename)
{
    //  Load entire file into memory as a chunk, then process it
    zconfig_t *self = NULL;
    zfile_t *file = zfile_new (NULL, filename);
    if (!file)
        return NULL;

    if (zfile_input (file) == 0) {
        zchunk_t *chunk = zfile_read (file, zfile_cursize (file), 0);
        if (chunk) {
            self = zconfig_chunk_load (chunk);
            zchunk_destroy (&chunk);
            if (self)
                self->file = file;
            zfile_close (file);
            file = NULL;        //  Config tree now owns file handle
        }
    }
    zfile_destroy (&file);
    return self;
}


//  --------------------------------------------------------------------------
//  Save a config tree to a specified ZPL text file, where a filename
//  "-" means dump to standard output.

int
zconfig_save (zconfig_t *self, const char *filename)
{
    assert (self);

    int rc = 0;
    if (streq (filename, "-"))
        //  "-" means write to stdout
        rc = zconfig_execute (self, s_config_save, stdout);
    else {
        FILE *file;
        file = fopen (filename, "w");
        if (file) {
            rc = zconfig_execute (self, s_config_save, file);
            fflush (file);
            fclose (file);

            //  If we saved back to original file, restat it so that
            //  the file does not appear as "changed"
            if (self->file && streq (filename, zconfig_filename (self)))
                zfile_restat (self->file);
        }
        else
            rc = -1;          //  File not writeable
    }
    return rc;
}

//  Save an item, polymorphic: if arg is a zchunk_t *, appends the
//  data to the chunk; else if arg is not null, writes data to the
//  arg as FILE *, or appends to the chunk data. If arg is null,
//  stores nothing. Returns data size, or -1 if memory is exhausted.

static int
s_config_printf (zconfig_t *self, void *arg, char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (!string)
        return -1;

    if (arg) {
        if (zchunk_is (arg))
            zchunk_append ((zchunk_t *) arg, string, strlen (string));
        else
            fprintf ((FILE *) arg, "%s", string);
    }
    size_t size = strlen (string);
    zstr_free (&string);
    if (size > INT_MAX)
        return -1;

    return (int) size;
}


static int
s_config_save (zconfig_t *self, void *arg, int level)
{
    assert (self);

    //  Store any comments on the item
    int size = 0;
    if (self->comments) {
        char *comment = (char *) zlist_first (self->comments);
        while (comment) {
            size += s_config_printf (self, arg, "#%s\n", comment);
            comment = (char *) zlist_next (self->comments);
        }
        size += s_config_printf (self, arg, "\n");
    }
    if (level > 0) {
        if (self->value)
            size += s_config_printf (self, arg,
                                     "%*s%s = \"%s\"\n", (level - 1) * 4, "",
                                     self->name? self->name: "(Unnamed)", self->value);
        else
            size += s_config_printf (self, arg,
                                     "%*s%s\n", (level - 1) * 4, "",
                                     self->name? self->name: "(Unnamed)");
    }
    return size;
}


//  --------------------------------------------------------------------------
//  Equivalent to zconfig_load, taking a format string instead of a fixed
//  filename.

zconfig_t *
zconfig_loadf (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *filename = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (filename) {
        zconfig_t *config = zconfig_load (filename);
        free (filename);
        return config;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Equivalent to zconfig_save, taking a format string instead of a fixed
//  filename.

int
zconfig_savef (zconfig_t *self, const char *format, ...)
{
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *filename = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (filename) {
        int rc = zconfig_save (self, filename);
        zstr_free (&filename);
        return rc;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Report filename used during zconfig_load, or NULL if none

const char *
zconfig_filename (zconfig_t *self)
{
    assert (self);
    if (self->file)
        return (zfile_filename (self->file, NULL));
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Reload config tree from same file that it was previously loaded from.
//  Returns 0 if OK, -1 if there was an error (and then does not change
//  existing data).

int
zconfig_reload (zconfig_t **self_p)
{
    assert (self_p);
    zconfig_t *self = *self_p;

    if (self->file) {
        zconfig_t *copy = zconfig_load (zfile_filename (self->file, NULL));
        if (copy) {
            //  Destroy old tree and install new one
            zconfig_destroy (self_p);
            *self_p = copy;
            return 0;
        }
    }
    return -1;              //  Not successful
}


//  --------------------------------------------------------------------------
//  Load a config tree from a memory chunk

zconfig_t *
zconfig_chunk_load (zchunk_t *chunk)
{
    //  Parse the chunk line by line
    zconfig_t *self = zconfig_new ("root", NULL);
    if (!self)
        return NULL;

    bool valid = true;
    int lineno = 0;
    char *data_ptr = (char *) zchunk_data (chunk);
    size_t remaining = zchunk_size (chunk);

    while (remaining) {
        //  Copy stuff into cur_line; not fastest but safest option
        //  since chunk may not be null terminated, etc.
        char *eoln = (char *) memchr (data_ptr, '\n', remaining);
        size_t cur_size;
        if (eoln)
            cur_size = eoln - data_ptr;
        else
            cur_size = remaining;

        if (cur_size > 1024)
            cur_size = 1024;
        char cur_line [1024 + 1];
        memcpy (cur_line, data_ptr, cur_size);
        cur_line [cur_size] = '\0';
        data_ptr = eoln? eoln + 1: NULL;
        remaining -= cur_size + (eoln? 1: 0);

        //  Trim line
        size_t length = strlen (cur_line);
        while (length && isspace ((byte) cur_line [length - 1]))
            cur_line [--length] = 0;

        //  Collect indentation level and name, if any
        lineno++;
        //  Handle whole-line comment if present
        if (cur_line [0] == '#') {
            if (!self->comments) {
                self->comments = zlist_new ();
                assert (self->comments);
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
                    assert (item);
                    item->value = value;
                }
                else {
                    zclock_log ("E (zconfig): (%d) indentation error", lineno);
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
    //  Either the whole ZPL stream is valid or none of it is
    if (!valid)
        zconfig_destroy (&self);
    return self;
}


//  Count and verify indentation level, -1 means a syntax error or overflow

static int
s_collect_level (char **start, int lineno)
{
    char *readptr = *start;
    while (*readptr == ' ')
        readptr++;
    ptrdiff_t level = (readptr - *start) / 4;
    if (level * 4 != readptr - *start) {
        zclock_log ("E (zconfig): (%d) indent 4 spaces at once", lineno);
        level = -1;
    }
    *start = readptr;
    if (level > INT_MAX)
        return -1;

    return (int) level;
}

//  Collect property name

static bool
s_is_namechar (char thischar)
{
    return (  isalnum (thischar)
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
    if (!name)
        return NULL;

    memcpy (name, readptr, length);
    name [length] = 0;

    if (length > 0
    && (name [0] == '/'
    ||  name [length - 1] == '/')) {
        zclock_log ("E (zconfig): (%d) '/' not valid at name start or end", lineno);
        free (name);
        name = NULL;
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
            zclock_log ("E (zconfig): (%d) invalid syntax '%s'", lineno, readptr);
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
                if (!value)
                    return NULL;

                memcpy (value, readptr + 1, value_length);
                value [value_length] = 0;
                rc = s_verify_eoln (endquote + 1, lineno);
            }
            else {
                zclock_log ("E (zconfig): (%d) missing %c", lineno, *readptr);
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
    if (rc) {
        free (value);
        value = NULL;
    }
    return value;
}


//  --------------------------------------------------------------------------
//  Save a config tree to a new memory chunk; the chunk 

zchunk_t *
zconfig_chunk_save (zconfig_t *self)
{
    assert (self);

    int size = s_config_execute (self, s_config_save, NULL, 0);
    //  Allow an extra byte so we can null-terminate the data
    zchunk_t *chunk = zchunk_new (NULL, size + 1);
    if (chunk) {
        s_config_execute (self, s_config_save, chunk, 0);
        //  This lets us treat the chunk data as a string
        zchunk_data (chunk) [zchunk_size (chunk)] = 0;
    }
    return chunk;
}


//  --------------------------------------------------------------------------
//  Create a new config tree from a null-terminated string

zconfig_t *
zconfig_str_load (const char *string)
{
    zchunk_t *chunk = zchunk_new (string, strlen (string));
    zconfig_t *config = zconfig_chunk_load (chunk);
    zchunk_destroy (&chunk);
    return config;
}


//  --------------------------------------------------------------------------
//  Save a config tree to a new null terminated string

char *
zconfig_str_save (zconfig_t *self)
{
    zchunk_t *chunk = zconfig_chunk_save (self);
    char *string = strdup ((char *) zchunk_data (chunk));
    zchunk_destroy (&chunk);
    return string;
}


//  --------------------------------------------------------------------------
//  Return true if a configuration tree was loaded from a file and that
//  file has changed in since the tree was loaded.

bool
zconfig_has_changed (zconfig_t *self)
{
    assert (self);
    if (self->file && zfile_has_changed (self->file))
        return true;
    else
        return false;
}


//  --------------------------------------------------------------------------
//  Add comment to config item before saving to disk. You can add as many
//  comment lines as you like. If you use a null format, all comments are
//  deleted.

void
zconfig_set_comment (zconfig_t *self, const char *format, ...)
{
    if (format) {
        if (!self->comments) {
            self->comments = zlist_new ();
            assert (self->comments);
            zlist_autofree (self->comments);
        }
        va_list argptr;
        va_start (argptr, format);
        char *string = zsys_vprintf (format, argptr);
        va_end (argptr);

        zlist_append (self->comments, string);
        zstr_free (&string);
    }
    else
        zlist_destroy (&self->comments);
}


//  --------------------------------------------------------------------------
//  Return comments of config item, as zlist.

zlist_t *
zconfig_comments (zconfig_t *self)
{
    assert (self);
    return self->comments;
}


//  --------------------------------------------------------------------------
//  Print the config file to open stream

void
zconfig_fprint (zconfig_t *self, FILE *file)
{
    zconfig_execute (self, s_config_save, file);
}


//  --------------------------------------------------------------------------
//  Print the config file to stdout

void
zconfig_print (zconfig_t *self)
{
    zconfig_fprint (self, stdout);
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
    assert (root);
    zconfig_t *section, *item;

    section = zconfig_new ("headers", root);
    assert (section);
    item = zconfig_new ("email", section);
    assert (item);
    zconfig_set_value (item, "some@random.com");
    item = zconfig_new ("name", section);
    assert (item);
    zconfig_set_value (item, "Justin Kayce");
    zconfig_putf (root, "/curve/secret-key", "%s", "Top Secret");
    zconfig_set_comment (root, "   CURVE certificate");
    zconfig_set_comment (root, "   -----------------");
    assert (zconfig_comments (root));
    zconfig_save (root, TESTDIR "/test.cfg");
    zconfig_destroy (&root);
    root = zconfig_load (TESTDIR "/test.cfg");
    if (verbose)
        zconfig_save (root, "-");
    assert (streq (zconfig_filename (root), TESTDIR "/test.cfg"));

    char *email = zconfig_get (root, "/headers/email", NULL);
    assert (email);
    assert (streq (email, "some@random.com"));
    char *passwd = zconfig_get (root, "/curve/secret-key", NULL);
    assert (passwd);
    assert (streq (passwd, "Top Secret"));

    zconfig_savef (root, "%s/%s", TESTDIR, "test.cfg");
    assert (!zconfig_has_changed (root));
    int rc = zconfig_reload (&root);
    assert (rc == 0);
    assert (!zconfig_has_changed (root));
    zconfig_destroy (&root);

    //  Test chunk load/save
    root = zconfig_new ("root", NULL);
    assert (root);
    section = zconfig_new ("section", root);
    assert (section);
    item = zconfig_new ("value", section);
    assert (item);
    zconfig_set_value (item, "somevalue");
    zconfig_t *search = zconfig_locate (root, "section/value");
    assert (search == item);
    zchunk_t *chunk = zconfig_chunk_save (root);
    assert (strlen ((char *) zchunk_data (chunk)) == 32);
    char *string = zconfig_str_save (root);
    assert (string);
    assert (streq (string, (char *) zchunk_data (chunk)));
    free (string);
    assert (chunk);
    zconfig_destroy (&root);

    root = zconfig_chunk_load (chunk);
    assert (root);
    char *value = zconfig_get (root, "/section/value", NULL);
    assert (value);
    assert (streq (value, "somevalue"));

    //  Test config can't be saved to a file in a path that doesn't
    //  exist or isn't writable
    rc = zconfig_savef (root, "%s/path/that/doesnt/exist/%s", TESTDIR, "test.cfg");
    assert (rc == -1);

    zconfig_destroy (&root);
    zchunk_destroy (&chunk);

    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end

    printf ("OK\n");
}

