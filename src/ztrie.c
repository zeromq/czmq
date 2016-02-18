/*  =========================================================================
    ztrie - simple trie for tokenizable strings

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
    This is a variant of a trie or prefix tree where all the descendants of a
    node have a common prefix of the string associated with that node. This
    implementation is specialized for strings that can be tokenized by a delimiter
    like a URL, URI or URN. Routes in the tree can be matched by regular expressions
    and by using capturing groups parts of a matched route can be easily obtained.
@discuss
    Note that the performance for pure string based matching is okay but on short
    strings zhash and zhashx are 3-4 times faster.
@end
*/

#include "czmq_classes.h"

#define MODE_INSERT 0
#define MODE_LOOKUP 1
#define MODE_MATCH  2

#define NODE_TYPE_STRING    0  //  Node with string token
#define NODE_TYPE_REGEX     1  //  Node with regex token
#define NODE_TYPE_PARAM     2  //  Node with named regex token and capturing group(s)
#define NODE_TYPE_ASTERISK  3  //  Node with an asterisk, to match routes of variable length

#define MIN_LEN(x,y) \
    y + ((x - y) & ((x - y) >>(sizeof(int) * CHAR_BIT - 1)))


// TODO: Move to a more appropriate location:
char *
s_strndup (const char *s, size_t size) {
    char *dup;
    char *end = (char *) memchr (s, '\0', size);
    if (end)
        size = end - 1 - s;     //  -1 to get the last char before '\0'
    dup = (char *) zmalloc (sizeof (char) * size + 1); //  +1 for trailing '\0'
    if (size) {
        memcpy (dup, s, size);
        dup [size] = '\0';
    }
    return dup;
}

//  Trie node, used internally only

typedef struct _ztrie_node_t {
    char *token;         //  Part of a path between to delemiters e.g. '/{token}/'
    int token_type;      //  Type of the token, string, regex, asterisk
    int token_len;       //  Number of characters of the token
    size_t path_len;     //  Length of the path/route including this token
    bool endpoint;       //  Has a path been added that routes to this node?
    size_t parameter_count;  //  How many regex parameters does this token contain?
    char **parameter_names;  //  Names of the regex parameters for easy access at matching time
    char **parameter_values; //  Values of the parameters
    char *asterisk_match;    //  Matched asterisk route
    zrex_t *regex;           //  Compiled regex
    void *data;              //  Custom arbitrary data assoziated with a route
    ztrie_destroy_data_fn *destroy_data_fn;  // Callback to destroy custom data
    zlistx_t *children;       //  List of all children to this route
    struct _ztrie_node_t *parent;  // Parent of this node
} ztrie_node_t;

//  --------------------------------------------------------------------------
//  Structure of our class

struct _ztrie_t {
    char delimiter;         //  Character that seperates the tokens of a route
    ztrie_node_t *root;     //  Root node of this tree
    ztrie_node_t *match;    //  Last match made by ztrie_matches
    zlistx_t *params;       //  List of regex parameters found during parsing
                            //  The list is kept globally to optimize performance.
};

//  Internal helper functions

static int
s_ztrie_node_compare (const void *item1, const void *item2) {
    assert (item1);
    assert (item2);
    ztrie_node_t *node1 = (ztrie_node_t *) item1;
    ztrie_node_t *node2 = (ztrie_node_t *) item2;

    int same = node1->token_type - node2->token_type;
    if (same == 0) {
        if (*node1->token == *node2->token  //  This achieves a small performance boost
            && node1->token_len == node2->token_len
            && strncmp (node1->token, node2->token, MIN_LEN (node1->token_len, node2->token_len)) == 0)
            return 0;
        else
            return -1;
    }
    else
         return same;
}


//  Create a new ztrie_node. If parent is not NULL the child will add itself
//  to the parents children.

static ztrie_node_t *
s_ztrie_node_new (ztrie_node_t *parent, char *token, int token_len, zlistx_t *param_keys, int token_type)
{
    ztrie_node_t *self = (ztrie_node_t *) zmalloc (sizeof (ztrie_node_t));
    assert (self);

    //  Initialize properties
    self->token = s_strndup (token, token_len);
    self->token_type = token_type;
    self->token_len = token_len;
    self->endpoint = false;
    self->parameter_count = 0;
    self->parameter_names = NULL;
    self->parameter_values = NULL;
    if (param_keys && zlistx_size (param_keys) > 0) {
        self->parameter_count = zlistx_size (param_keys);
        self->parameter_names = (char **) malloc (sizeof (char *) * self->parameter_count);
        self->parameter_values = (char **) malloc (sizeof (char *) * self->parameter_count);
        char *key = (char *) zlistx_first (param_keys);
        size_t index;
        for (index = 0; index < zlistx_size (param_keys); index++) {
            self->parameter_names [index] = key;
            self->parameter_values [index] = NULL;
            key = (char *) zlistx_next (param_keys);
        }
    }
    if (self->token_type == NODE_TYPE_REGEX || self->token_type == NODE_TYPE_PARAM)
        self->regex = zrex_new (self->token);
    self->data = NULL;
    //  Initialize relationships
    self->parent = parent;
    if (self->parent) {
        zlistx_add_end (self->parent->children, self);
        //  Sort regexes to the end to avoid conlficts
        zlistx_sort (self->parent->children);
    }
    size_t parent_path_len = self->parent? self->parent->path_len: 0;
    self->path_len = parent_path_len + strlen (self->token) + 1; // +1 path delimiter
    self->children = zlistx_new ();
    zlistx_set_comparator (self->children, s_ztrie_node_compare);

    return self;
}


//  Destroy the ztrie_node.

static void
s_ztrie_node_destroy (ztrie_node_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        ztrie_node_t *self = *self_p;

        //  Free class properties
        zstr_free (&self->token);
        zstr_free (&self->asterisk_match);
        if (self->parameter_count > 0) {
            size_t index;
            for (index = 0; index < self->parameter_count; index++) {
                free (self->parameter_names [index]);
                if (self->parameter_values [index])
                    free (self->parameter_values [index]);
            }
            free (self->parameter_names);
            free (self->parameter_values);
        }
        if (self->token_type == NODE_TYPE_REGEX || self->token_type == NODE_TYPE_PARAM)
            zrex_destroy (&self->regex);
        zlistx_destroy (&self->children);
        if (self->data && self->destroy_data_fn)
            (self->destroy_data_fn) (&self->data);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  Update the value of a regex parameter at position.

static void
s_ztrie_node_update_param (ztrie_node_t *self, int pos, const char *value)
{
    assert (self);
    zstr_free (&self->parameter_values [pos - 1]);
    self->parameter_values [pos - 1] = strdup (value);
}


//  --------------------------------------------------------------------------
//  Creates a new ztrie.

ztrie_t *
ztrie_new (char delimiter)
{
    ztrie_t *self = (ztrie_t *) zmalloc (sizeof (ztrie_t));
    assert (self);

    //  Initialize properties
    if (delimiter)
        self->delimiter = delimiter;
    else
        self->delimiter = '/';
    self->root = s_ztrie_node_new (NULL, "", 0,  NULL, NODE_TYPE_STRING);
    self->match = NULL;
    self->params = zlistx_new ();

    return self;
}

//  Destroy all children of a node

static void
s_ztrie_destroy_children (ztrie_node_t *node)
{
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (node->children);
    while (child) {
        s_ztrie_destroy_children (child);
        s_ztrie_node_destroy (&child);
        child = (ztrie_node_t *) zlistx_next (node->children);
    }
}

//  --------------------------------------------------------------------------
//  Destroy the ztrie.

void
ztrie_destroy (ztrie_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        ztrie_t *self = *self_p;

        //  Free class properties
        s_ztrie_destroy_children (self->root);
        s_ztrie_node_destroy (&self->root);
        zlistx_destroy (&self->params);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  String compare token

static ztrie_node_t *
s_ztrie_compare_token (ztrie_node_t *parent, char *token, int len)
{
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (parent->children);
    while (child) {
        if (child->token_len == len &&
            strncmp (child->token, token, MIN_LEN (child->token_len, len)) == 0)
            return child;
        child = (ztrie_node_t *) zlistx_next (parent->children);
    }
    return NULL;
}

//  String compare token or evaluate regexes

static ztrie_node_t *
s_ztrie_matches_token (ztrie_node_t *parent, char *token, int len)
{
    char firstbyte = *token;
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (parent->children);
    while (child) {
        if (child->token_type == NODE_TYPE_STRING) {
            if (firstbyte == *child->token  //  This achieves a small performance boost
                && child->token_len == len
                && strncmp (child->token, token, MIN_LEN (child->token_len, len)) == 0)
                    return child;
        }
        else
        if (child->token_type == NODE_TYPE_ASTERISK) {
            child->asterisk_match = strdup (token);
            return child;
        }
        else {
            //  Need to copy token as zrex_matches expects '\0' terminated string
            char *token_term = s_strndup (token, len);
            if (zrex_matches (child->regex, token_term)) {
                if (child->token_type == NODE_TYPE_PARAM) {
                    //  One hit means no capturing group was defined
                    //  More than one hit indicates that at least on capturing group.
                    //  In this case only the values of the capturing groups are considered.
                    if (zrex_hits (child->regex) == 1)
                        s_ztrie_node_update_param (child, 1, zrex_hit (child->regex, 0));
                    else
                    if (zrex_hits (child->regex) > 1) {
                        int index;
                        for (index = 1; index < zrex_hits (child->regex); index++)
                            s_ztrie_node_update_param (child, index, zrex_hit (child->regex, index));
                    }
                }
                free (token_term);
                return child;
            }
            free (token_term);
        }
        child = (ztrie_node_t *) zlistx_next (parent->children);
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Parses a path bytewise trying to find matches for path tokens. Depending
//  on the mode there are different behaviors on,
//  how the tokens are compared:
//    MODE_INSERT:  All tokens are compared as strings.
//    MODE_LOOKUP:  All tokens are compared as strings.
//    MODE_MATCH:   Node tokens of NODE_TYPE_STRING tokens are compared as strings,
//                  otherwise the tokens are matched against the nodes regex.
//  how a mismatch is handled:
//    MODE_INSERT: creates a new node and attaches it to the common prefix for
//                 the given path, repeat for the remaining path tokens.
//                 returns the node that has been attached last.
//    MODE_LOOKUP: returns NULL if the comparison failed.
//    MODE_MATCH:  returns NULL if the comparison failed.

static ztrie_node_t *
s_ztrie_parse_path (ztrie_t *self, const char *path, int mode)
{
    int state = 0;
    char *needle, *beginToken = NULL, *beginRegex = NULL;
    ztrie_node_t *parent = self->root;
    if (zlistx_size (self->params) > 0)
        zlistx_purge (self->params);

    size_t len = strlen (path);
    needle = (char *) path;
    char *needle_stop = needle + len;
    //  Ignore trailing delimiter
    if (needle [len-1] == self->delimiter)
        needle_stop -= 1;
    while (needle < needle_stop + 1) {
        //  It is valid not to have an delimiter at the end of the path
        if (*needle == self->delimiter || needle == needle_stop) {
            //  Token starts with delimiter ignore everything that comes before
            if (state == 0) {
                beginToken = needle + 1;
                state++;
                if (mode == MODE_INSERT || mode == MODE_LOOKUP)
                    // Increment so regexes are parsed which is only relevant
                    // during INSERT or LOOKUP. Using different states gives a small
                    // performance boost for matching.
                    state++;
            }
            //  Token ends with delimiter.
            else
            if (state < 3) {
                int matchType = zlistx_size (self->params) > 0? NODE_TYPE_PARAM:
                                    beginRegex? NODE_TYPE_REGEX: NODE_TYPE_STRING;
                char *matchToken = beginRegex? beginRegex: beginToken;
                int matchTokenLen = (int) (needle - matchToken) - (beginRegex? 1: 0);
                //  Illegal token
                if (matchTokenLen == 0)
                    return NULL;
                ztrie_node_t *match = NULL;
                //  Asterisk nodes are only allowed at the end of a route
                if (needle == needle_stop && *matchToken == '*') {
                    if (zlistx_size (parent->children) == 0) {
                        matchType = NODE_TYPE_ASTERISK;
                        matchToken = needle - 1;
                        matchTokenLen = 1;
                    }
                    //  Asterisk must be a leaf in the tree
                    else
                        return NULL;
                }
                else {
                    matchType = zlistx_size (self->params) > 0? NODE_TYPE_PARAM:
                                        beginRegex? NODE_TYPE_REGEX: NODE_TYPE_STRING;
                    matchToken = beginRegex? beginRegex: beginToken;
                    matchTokenLen = (int) (needle - matchToken) - (beginRegex? 1: 0);
                }

                //  In insert and lookup mode only do a string comparison
                if (mode == MODE_INSERT || mode == MODE_LOOKUP)
                    match = s_ztrie_compare_token (parent, matchToken, matchTokenLen);
                else
                //  Otherwise evaluate regexes
                if (mode == MODE_MATCH)
                    match = s_ztrie_matches_token (parent, matchToken, matchTokenLen);

                //  Mismatch behavior depends on mode
                if (!match) {
                    //  Append to common prefix
                    if (mode == MODE_INSERT) {
                        //  It's not allowed to append on asterisk
                        if (parent->token_type == NODE_TYPE_ASTERISK ||
                                (zlistx_size (parent->children) == 1 &&
                                ((ztrie_node_t *) (zlistx_first (parent->children)))->token_type == NODE_TYPE_ASTERISK))
                            return NULL;
                        parent = s_ztrie_node_new (parent, matchToken, matchTokenLen, self->params, matchType);
                    }
                    else
                    //  No match for path found
                    if (mode == MODE_MATCH || mode == MODE_LOOKUP)
                        return NULL;
                }
                //  If a match has been found it becomes the parent for next path token
                else {
                    parent = match;
                    //  In case a asterisk match has been made skip the rest of the route
                    if (parent->token_type == NODE_TYPE_ASTERISK)
                        break;
                }
                //  Cleanup for next token
                beginRegex = NULL;
                if (zlistx_size (self->params) > 0)
                    zlistx_purge (self->params);
                //  Token end equals token begin
                beginToken = needle + 1;
            }
        }
        else
        //  regex starts with '{'
        if (state == 2 && *needle == '{') {
            beginRegex = needle + 1;
            state++;
        }
        else
        //  in the middle of the regex. Found a named regex.
        if (state == 3 && (*needle == ':')) {
            zlistx_add_end (self->params, s_strndup (beginRegex, needle - beginRegex));
            beginRegex = needle + 1;

        }
        else
        // regex ends with {
        if (state == 3 && *needle == '}') {
            state--;
        }
        needle++;
    }

    //  In matching mode the discovered node must be an endpoint
    if (parent && mode == MODE_MATCH && !parent->endpoint)
        return NULL;

    return parent;
}


//  --------------------------------------------------------------------------
//  Inserts a new route into the trie and attaches the data. Returns -1
//  if the route already exists, otherwise 0. This method takes ownership of
//  the provided data.

int
ztrie_insert_route (ztrie_t *self, const char *path, void *data, ztrie_destroy_data_fn destroy_data_fn)
{
    assert (self);
    ztrie_node_t *node = s_ztrie_parse_path (self, path, MODE_INSERT);
    //  If the returned node has no endpoint, a new route can be assigned to it.
    if (node && !node->endpoint) {
        node->endpoint = true;
        node->data = data;
        node->destroy_data_fn = destroy_data_fn;
        return 0;
    }
    //  If the returned node has an endpoint, a route has already assigned to it.
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Removes a route from the trie and destroys its data. Returns -1 if the
//  route does not exists, otherwise 0.

int
ztrie_remove_route (ztrie_t *self, const char *path)
{
    assert (self);
    ztrie_node_t *match = s_ztrie_parse_path (self, path, MODE_LOOKUP);
    //  The path did match a node which is endpoint to a route
    if (match && match->endpoint) {
        //  This node is part of other routes, thus it cannot destroy it
        if (zlistx_size (match->children) > 0) {
            match->endpoint = false;
            if (match->data && match->destroy_data_fn)
                (match->destroy_data_fn) (&match->data);
        }
        //  If this node is not part of other routes, destroy it
        else {
            //  Delete match from parent's children before destroying
            void *handle = zlistx_find (match->parent->children, match);
            assert (handle);
            zlistx_delete (match->parent->children, handle);
            s_ztrie_node_destroy (&match);
        }
        return 0;
    }
    //  If there is no match or the match is not endpoint to a route, fail
    else
        return -1;
}

//  --------------------------------------------------------------------------
//  Returns true if the path matches a route in the tree, otherwise false.

bool
ztrie_matches (ztrie_t *self, const char *path)
{
    assert (self);
    self->match = s_ztrie_parse_path (self, path, MODE_MATCH);
    return self->match? true: false;
}


//  --------------------------------------------------------------------------
//  Returns the data of a matched route from last ztrie_matches. If the path
//  did not match, returns NULL. Do not delete the data as it's owned by
//  ztrie.

void *
ztrie_hit_data (ztrie_t *self)
{
    assert (self);
    if (self->match)
        return self->match->data;
    return NULL;
}


//  --------------------------------------------------------------------------
//  Returns the count of parameters that a matched route has.

size_t
ztrie_hit_parameter_count (ztrie_t *self)
{
    size_t count = 0;
    ztrie_node_t *node = self->match;
    while (node) {
        count += node->parameter_count;
        node = node->parent;
    }
    return count;
}

//  --------------------------------------------------------------------------
//  Returns the parameters of a matched route with named regexes from last
//  ztrie_matches. If the path did not match or the route did not contain any
//  named regexes, returns NULL. The caller is responseable for destroy the map.

zhashx_t *
ztrie_hit_parameters (ztrie_t *self)
{
    assert (self);
    if (self->match) {
        zhashx_t *route_parameters = zhashx_new ();
        ztrie_node_t *node = self->match;
        while (node) {
            size_t index;
            for (index = 0; index < node->parameter_count; index++)
                zhashx_insert (route_parameters,
                               node->parameter_names [index],
                               (void *) node->parameter_values [index]);
            node = node->parent;
        }
        return route_parameters;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Returns the asterisk matched part of a route, if there has been no match
//  or no asterisk match, returns NULL.

const char *
ztrie_hit_asterisk_match (ztrie_t *self)
{
    assert (self);
    if (self->match)
        return self->match->asterisk_match;
    return NULL;
}

//  --------------------------------------------------------------------------
//  Print properties of the ztrie object.
//

static void
s_ztrie_print_tree_line (ztrie_node_t *self, bool end_line)
{
    if (self->parent) {
        s_ztrie_print_tree_line (self->parent, false);
        if (zlistx_tail (self->parent->children) == self) {
            if (end_line)
                printf ("`-- ");
            else
                printf ("    ");
        }
        else {
            if (end_line)
                printf ("+-- ");
            else
                printf ("|   ");
        }
        if (end_line) {
            char *is_endpoint = self->endpoint? "true": "false";
            printf ("%s (params: %zu, endpoint: %s, type: %d)\n",
                self->token, self->parameter_count, is_endpoint, self->token_type);
        }
    }
}

static void
s_ztrie_print_tree (ztrie_node_t *self)
{
    //  Print tree like structure
    s_ztrie_print_tree_line (self, true);
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (self->children);
    while (child) {
        s_ztrie_print_tree (child);
        child = (ztrie_node_t *) zlistx_next (self->children);
    }
}

void
ztrie_print (ztrie_t *self)
{
    assert (self);
    s_ztrie_print_tree (self->root);
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
ztrie_test (bool verbose)
{
    printf (" * ztrie: ");
    //  @selftest
    //  Create a new trie for matching strings that can be tokenized by a slash
    //  (e.g. URLs minus the protocol, address and port).
    ztrie_t *self = ztrie_new ('/');
    assert (self);

    int ret = 0;

    //  Let's start by inserting a couple of routes into the trie.
    //  This one is for the route '/foo/bar' the slash at the beginning of the
    //  route is important because everything before the first delimiter will be
    //  discarded. A slash at the end of a route is optional though. The data
    //  associated with this node is passed without destroy function which means
    //  it must be destroyed by the caller.
    int foo_bar_data = 10;
    ret = ztrie_insert_route (self, "/foo/bar", &foo_bar_data, NULL);
    assert (ret == 0);

    //  Now suppose we like to match all routes with two tokens that start with
    //  '/foo/' but aren't '/foo/bar'. This is possible by using regular
    //  expressions which are enclosed in an opening and closing curly bracket.
    //  Tokens that contain regular  expressions are always match after string
    //  based tokens.
    //  Note: There is no order in which regular expressions are sorted thus
    //  if you enter multiple expressions for a route you will have to make
    //  sure they don't have overlapping results. For example '/foo/{[^/]+}'
    //  and '/foo/{\d+} having could turn out badly.
    int foo_other_data = 100;
    ret = ztrie_insert_route (self, "/foo/{[^/]+}", &foo_other_data, NULL);
    assert (ret == 0);

    //  Regular expression are only matched against tokens of the same level.
    //  This allows us to append to are route with a regular expression as if
    //  it were a string.
    ret = ztrie_insert_route (self, "/foo/{[^/]+}/gulp", NULL, NULL);
    assert (ret == 0);

    //  Routes are identified by their endpoint, which is the last token of the route.
    //  It is possible to insert routes for a node that already exists but isn't an
    //  endpoint yet. The delimiter at the end of a route is optional and has no effect.
    ret = ztrie_insert_route (self, "/foo/", NULL, NULL);
    assert (ret == 0);

    //  If you try to insert a route which already exists the method will return -1.
    ret = ztrie_insert_route (self, "/foo", NULL, NULL);
    assert (ret == -1);

    //  It is not allowed to insert routes with empty tokens.
    ret = ztrie_insert_route (self, "//foo", NULL, NULL);
    assert (ret == -1);

    //  Everything before the first delimiter is ignored so 'foo/bar/baz' is equivalent
    //  to '/bar/baz'.
    ret = ztrie_insert_route (self, "foo/bar/baz", NULL, NULL);
    assert (ret == 0);
    ret = ztrie_insert_route (self, "/bar/baz", NULL, NULL);
    assert (ret == -1);

    //  Of course you are allowed to remove routes, in case there is data associated with a
    //  route and a destroy data function has been supplied that data will be destroyed.
    ret = ztrie_remove_route (self, "/foo");
    assert (ret == 0);

    //  Removing a non existent route will  as well return -1.
    ret = ztrie_remove_route (self, "/foo");
    assert (ret == -1);

    //  Removing a route with a regular expression must exactly match the entered one.
    ret = ztrie_remove_route (self, "/foo/{[^/]+}");
    assert (ret == 0);

    //  Next we like to match a path by regular expressions and also extract matched
    //  parts of a route. This can be done by naming the regular expression. The name of a
    //  regular expression is entered at the beginning of the curly brackets and separated
    //  by a colon from the regular expression. The first one in this examples is named
    //  'name' and names the expression '[^/]'. If there is no capturing group defined in
    //  the expression the whole matched string will be associated with this parameter. In
    //  case you don't like the get the whole matched string use a capturing group, like
    //  it has been done for the 'id' parameter. This is nice but you can even match as
    //  many parameter for a token as you like. Therefore simply put the parameter names
    //  separated by colons in front of the regular expression and make sure to add a
    //  capturing group for each parameter. The first parameter will be associated with
    //  the first capturing and so on.
    char *data = (char *) malloc (80);
    sprintf (data, "%s", "Hello World!");
    ret = ztrie_insert_route (self, "/baz/{name:[^/]+}/{id:--(\\d+)}/{street:nr:(\\a+)(\\d+)}", data, NULL);
    assert (ret == 0);

    //  There is a lot you can do with regular expression but matching routes
    //  of arbitrary length wont work. Therefore we make use of the asterisk
    //  operator. Just place it at the end of your route, e.g. '/config/bar/*'.
    ret = ztrie_insert_route (self, "/config/bar/*", NULL, NULL);
    assert (ret == 0);

    //  Appending to an asterisk as you would to with a regular expression
    //  isn't valid.
    ret = ztrie_insert_route (self, "/config/bar/*/bar", NULL, NULL);
    assert (ret == -1);

    //  The asterisk operator will only work as a leaf in the tree. If you
    //  enter an asterisk in the middle of your route it will simply be
    //  interpreted as a string.
    ret = ztrie_insert_route (self, "/test/*/bar", NULL, NULL);
    assert (ret == 0);

    //  If a parent has an asterisk as child it is not allowed to have
    //  other siblings.
    ret = ztrie_insert_route (self, "/config/bar/foo/glup", NULL, NULL);
    assert (ret != 0);

    //  Test matches
    bool hasMatch = false;

    //  The route '/bar/foo' will fail to match as this route has never been inserted.
    hasMatch = ztrie_matches (self, "/bar/foo");
    assert (!hasMatch);

    //  The route '/foo/bar' will match and we can obtain the data associated with it.
    hasMatch = ztrie_matches (self, "/foo/bar");
    assert (hasMatch);
    int foo_bar_hit_data = *((int *) ztrie_hit_data (self));
    assert (foo_bar_data == foo_bar_hit_data);

    //  This route is part of another but is no endpoint itself thus the matches will fail.
    hasMatch = ztrie_matches (self, "/baz/blub");
    assert (!hasMatch);

    //  This route will match our named regular expressions route. Thus we can extract data
    //  from the route by their names.
    hasMatch = ztrie_matches (self, "/baz/blub/--11/abc23");
    assert (hasMatch);
    char *match_data = (char *) ztrie_hit_data (self);
    assert (streq ("Hello World!", match_data));
    zhashx_t *parameters = ztrie_hit_parameters (self);
    assert (zhashx_size (parameters) == 4);
    assert (streq ("blub", (char *) zhashx_lookup (parameters, "name")));
    assert (streq ("11", (char *) zhashx_lookup (parameters, "id")));
    assert (streq ("abc", (char *) zhashx_lookup (parameters, "street")));
    assert (streq ("23", (char *) zhashx_lookup (parameters, "nr")));
    zhashx_destroy (&parameters);

    //  This will match our asterisk route '/config/bar/*'. As the result we
    //  can obtain the asterisk matched part of the route.
    hasMatch = ztrie_matches (self, "/config/bar/foo/bar");
    assert (hasMatch);
    assert (streq (ztrie_hit_asterisk_match (self), "foo/bar"));

    zstr_free (&data);
    ztrie_destroy (&self);
    //  @end

    printf ("OK\n");
}
