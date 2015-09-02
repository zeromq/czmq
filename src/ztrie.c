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

#include "../include/czmq.h"
#include "czmq_classes.h"

#define MODE_INSERT 0
#define MODE_LOOKUP 1
#define MODE_MATCH  2

#define NODE_TYPE_STRING 0  //  Node with string token
#define NODE_TYPE_REGEX  1  //  Node with regex token
#define NODE_TYPE_PARAM  2  //  Node with regex token and capturing group

#define MIN_LEN(x,y) \
    y + ((x - y) & ((x - y) >>(sizeof(int) * CHAR_BIT - 1)))

//  Trie node, used internally only

typedef struct _ztrie_node_t {
    char *token;         //  Part of an path between to delemiters e.g. '/{token}/'
    int token_type;      //  Type of the token, string or regex
    int token_len;       //  Number of characters in the token
    size_t path_len;     //  Length of the path including this token
    bool endpoint;       //  Has a path been added that routes to this node?
    size_t parameter_count;  //  How many regex parameters does this token contain?
    char **parameter_names;  //  Names of the regex parameters for easy access at matching time
    char **parameter_values; //  Values of the parameters
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
    self->token = strndup (token, token_len);
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
        for (int i = 0; i < zlistx_size (param_keys); i++) {
            self->parameter_names [i] = key;
            self->parameter_values [i] = NULL;
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
    size_t parent_path_len = self->parent ? self->parent->path_len : 0;
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
        free (self->token);
        if (self->parameter_count > 0) {
            for (int i = 0; i < self->parameter_count; i++) {
                free (self->parameter_names [i]);
                if (self->parameter_values [i])
                    free (self->parameter_values [i]);
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
    unsigned int i;
    char firstbyte = *token;
    ztrie_node_t *child = (ztrie_node_t *) zlistx_first (parent->children);
    while (child) {
        if (child->token_type == NODE_TYPE_STRING) {
            if (firstbyte == *child->token  //  This achieves a small performance boost
                && child->token_len == len
                && strncmp (child->token, token, MIN_LEN (child->token_len, len)) == 0)
                    return child;
        }
        else {
            //  Need to copy token as zrex_matches expects '\0' terminated string
            char *token_term = strndup (token, len);
            if (zrex_matches (child->regex, token_term)) {
                if (child->token_type == NODE_TYPE_PARAM) {
                    //  One hit means no capturing group was defined
                    //  More than one hit indicates that at least on capturing group.
                    //  In this case only the values of the capturing groups are considered.
                    if (zrex_hits (child->regex) == 1)
                        s_ztrie_node_update_param (child, 1, zrex_hit (child->regex, 0));
                    else
                    if (zrex_hits (child->regex) > 1)
                        for (i = 1; i < zrex_hits (child->regex); i++)
                            s_ztrie_node_update_param (child, i, zrex_hit (child->regex, i));
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
s_ztrie_parse_path (ztrie_t *self, char *path, int mode)
{
    int state = 0;
    char *needle, *beginToken = NULL, *beginRegex = NULL;
    ztrie_node_t *parent = self->root;
    if (zlistx_size (self->params) > 0)
        zlistx_purge (self->params);

    int len = strlen (path);
    needle = path;
    char *needle_stop = needle + len;
    while (needle < needle_stop + 1) {
        //  It is valid not to have an delimiter at the end of the path
        if (*needle == self->delimiter || needle == needle_stop) {
            //  token starts with delimiter ignore everything that comes before
            if (state == 0) {
                beginToken = needle + 1;
                state++;
                if (mode == MODE_INSERT || mode == MODE_LOOKUP)
                    // Increment so regexes are parsed which is only relevant
                    // during INSERT or LOOKUP. Using different states gives a small
                    // performance boost for matching.
                    state++;
            }
            //  token ends with delimiter.
            else
            if (state < 3) {
                int matchType = zlistx_size (self->params) > 0 ? NODE_TYPE_PARAM :
                                    beginRegex ? NODE_TYPE_REGEX : NODE_TYPE_STRING;
                char *matchToken = beginRegex ? beginRegex : beginToken;
                int matchTokenLen = needle - matchToken - (beginRegex ? 1 : 0);
                ztrie_node_t *match = NULL;
                //  In insert mode only do a string comparison
                if (mode == MODE_INSERT || mode == MODE_LOOKUP)
                    match = s_ztrie_compare_token (parent, matchToken, matchTokenLen);
                //  Otherwise evaluate regexes
                else
                    match = s_ztrie_matches_token (parent, matchToken, matchTokenLen);
                //  Mismatch behavior depends on mode
                if (!match) {
                    //  Append to common prefix
                    if (mode == MODE_INSERT) {
                        match = s_ztrie_node_new (parent, matchToken, matchTokenLen, self->params, matchType);
                    }
                    //  No match for path found
                    if (mode == MODE_MATCH || mode == MODE_LOOKUP) {
                        parent = NULL;
                        break;
                    }
                }
                //  If a match has been found it becomes the parent for next path token
                if (match)
                    parent = match;
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
            zlistx_add_end (self->params, strndup (beginRegex, needle - beginRegex));
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
ztrie_insert_route (ztrie_t *self, char *path, void *data, ztrie_destroy_data_fn *destroy_data_fn)
{
    assert (self);
    ztrie_node_t *node = s_ztrie_parse_path (self, path, MODE_INSERT);
    //  If the returned node has no endpoint, a new route can be assigned to it.
    if (!node->endpoint) {
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
ztrie_remove_route (ztrie_t *self, char *path)
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
ztrie_matches (ztrie_t *self, char *path)
{
    assert (self);
    self->match = s_ztrie_parse_path (self, path, MODE_MATCH);
    return self->match ? true : false;
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
            for (int i = 0; i < node->parameter_count; i++)
                zhashx_insert (route_parameters,
                              node->parameter_names [i],
                              (void *) node->parameter_values [i]);
            node = node->parent;
        }
        return route_parameters;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Print properties of the ztrie object.
//
static void
s_ztrie_print_tree_line (ztrie_node_t *self, bool isEOL)
{
    if (self->parent) {
        s_ztrie_print_tree_line (self->parent, false);
        if (zlistx_tail (self->parent->children) == self) {
            if (isEOL)
                printf ("\u2514\u2500\u2500 ");
            else
                printf ("    ");
        }
        else {
            if (isEOL)
                printf ("\u251C\u2500\u2500 ");
            else
                printf ("\u2502   ");
        }
        if (isEOL) {
            char *isEndpoint = self->endpoint ? "true" : "false";
            printf ("%s (params: %zu, endpoint: %s, type: %d)\n", self->token,
                                                                 self->parameter_count,
                                                                 isEndpoint,
                                                                 self->token_type);
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
    //  route is importent because everything before will be discarded. The
    //  slash at the end of the route is optional. The data associated with this
    //  node is passed without destroy function which means it must be destroyed
    //  by us.
    int foo_bar_data = 10;
    ret = ztrie_insert_route (self, "/foo/bar", &foo_bar_data, NULL);
    assert (ret == 0);

    //  Now suppose we like to match all routes that start with '/foo' but aren't
    //  '/foo/bar'. This is posssible by using regular expressions which are enclosed
    //  in an opening and closing curly bracket. Regular expression for a route are
    //  always match after all the pure string based like '/foo/bar' have been matched.
    //  Note there is no order is you enter multiple expression for a route which may
    //  have overlapping results.
    int foo_other_data = 100;
    ret = ztrie_insert_route (self, "/foo/{[^/]+}", &foo_other_data, NULL);
    assert (ret == 0);

    //  Routes are identified by their endpoint, which is the last matched node. It is
    //  possible to insert routes for a node that already exists but isn't an endpoint yet.
    ret = ztrie_insert_route (self, "/foo", NULL, NULL);
    assert (ret == 0);

    //  If you try to insert a route which already exists the method will return -1.
    ret = ztrie_insert_route (self, "/foo", NULL, NULL);
    assert (ret == -1);

    //  Of course you are allowed to remove routes in case there is data associated with a
    //  route and a destroy data function has been supplied that data will be destroyed.
    ret = ztrie_remove_route (self, "/foo");
    assert (ret == 0);

    //  Removing a non existent route will return a -1.
    ret = ztrie_remove_route (self, "/foo");
    assert (ret == -1);

    //  Removing a route with regular expression must exaclty match the entered.
    ret = ztrie_remove_route (self, "/foo/{[^/]+}");
    assert (ret == 0);

    //  Next we like to match a path by regular expressions and also extract the matched
    //  parts of a route. This can be done by naming the regular expression. The first one is
    //  named 'name' and is seperated by a colon. If there is no capturing group defined in the
    //  regular expression the whole matched string will be associated with this parameter. In case
    //  you don't like the get the whole matched string use a capturing group like it's done with
    //  the 'id' parameter. This is nice but you can even match as many parameter for a token as
    //  you like. Therefore simply put the parameter names seperated by colons in front of the
    //  regular expression and make sure to add a capturing group for each parameter. The first
    //  parameter will be associated with the first capturing and so on.
    char *data = (char *) malloc (80);
    sprintf (data, "%s", "Hello World!");
    ret = ztrie_insert_route (self, "/baz/{name:[^/]+}/{id:--(\\d+)}/{street:nr:(\\a+)(\\d+)}", data, NULL);
    assert (ret == 0);

    //  Test matches
    bool hasMatch = false;

    //  The first match will fail as this route has never been inserted.
    hasMatch = ztrie_matches (self, "/bar/foo");
    assert (!hasMatch);

    //  The '/foo/bar' will match and we can obtain the data associated with it.
    hasMatch = ztrie_matches (self, "/foo/bar");
    assert (hasMatch);
    int foo_bar_hit_data = *((int *) ztrie_hit_data (self));
    assert (foo_bar_data == foo_bar_hit_data);

    //  This route is part of another but is no endpoint itself thus the matches will fail.
    hasMatch = ztrie_matches (self, "/baz/blub");
    assert (!hasMatch);

    //  Now we will match a patch with regular expressions and extract data from the matched route.
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

    free (data);
    ztrie_destroy (&self);
    //  @end

    printf ("OK\n");
}
