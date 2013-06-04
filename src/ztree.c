/*  =========================================================================
    ztree - generic type-free red-black tree container

    -------------------------------------------------------------------------
    Copyright (c) 2013 Stephen Hemminger <stephen@networkplumber.org>
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

    Derived from Redblack balanced tree algorithm
    Copyright (C) Damian Ivereigh 2000
    Licensed GNUL Lesser General Public License (version 2.1 or later).
*/


/*
@header
    Red black tree container
@discuss
@end
*/

#include "../include/czmq.h"

/*
   OK here we go, the balanced tree stuff. The algorithm is the
   fairly standard red/black taken from "Introduction to Algorithms"
   by Cormen, Leiserson & Rivest. Maybe one of these days I will
   fully understand all this stuff.
  
   Basically a red/black balanced tree has the following properties:-
   1) Every node is either red or black (colour is RED or BLACK)
   2) A leaf (NULL pointer) is considered black
   3) If a node is red then its children are black
   4) Every path from a node to a leaf contains the same no
      of black nodes
  
   3) & 4) above guarantee that the longest path (alternating
   red and black nodes) is only twice as long as the shortest
   path (all black nodes). Thus the tree remains fairly balanced.
*/

// Tree node, used internally

typedef struct _node_t {
    struct _node_t *left, *right, *up;
    enum nodecolour { black, red } colour;
    char *key;                  //  User's key
    void *value;                //      and datum
    zhash_free_fn *free_fn;     //  Value free function if any
} node_t;

struct _ztree_t {
    node_t *root;               //  Red-black tree root
    ztree_compare_fn *compare;  //  Comparison function
    bool autofree;              //  If true, free values in destructor
};

// Dummy (sentinel) node, so that we can make X->left->up = X
// We then use this instead of NULL to mean the top or bottom
// end of the rb tree. It is a black node.

static node_t s_null = { .colour = black };
#define ZTNULL (&s_null)

//  --------------------------------------------------------------------------
//  Tree constructor

ztree_t *
ztree_new (ztree_compare_fn *compare)
{
    assert (compare);
    
    ztree_t *self = (ztree_t *) zmalloc (sizeof (ztree_t));
    if (self) {
        self->compare = compare;
        self->root = ZTNULL;
    }
    return self;
}

static void
s_free_value (ztree_t *self, node_t *x)
{
    if (x->free_fn)
        (x->free_fn) (x->value);
    else 
    if (self->autofree)
        free (x->value);
}

//  Local helper function for recursive tree destruction

static void
s_destroy (ztree_t *self, node_t *x)
{
    if (x == ZTNULL)
        return;

    s_destroy (self, x->left);
    s_destroy (self, x->right);

    free (x->key);
    s_free_value (self, x);
    free (x);
}

//  --------------------------------------------------------------------------
//  Tree destructor

void
ztree_destroy (ztree_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        ztree_t *self = *self_p;
        s_destroy (self, self->root);
        free (self);
        *self_p = NULL;
    }
}

/*  Local helper function
  
   Rotate our tree thus:-
  
               X        rb_left_rotate (X)--->            Y
             /   \                                     /   \
            A     Y     <---rb_right_rotate (Y)        X     C
                /   \                               /   \
               B     C                             A     B
  
   N.B. This does not change the ordering.
  
   We assume that neither X or Y is NULL
*/

static void
s_left_rotate (ztree_t *tree, node_t *x)
{
    node_t *y;

    assert (x != ZTNULL);
    assert (x->right != ZTNULL);

    y = x->right; // set Y 

    // Turn Y's left subtree into X's right subtree (move B)
    x->right = y->left;

    // If B is not null, set it's parent to be X 
    if (y->left != ZTNULL)
        y->left->up = x;

    // Set Y's parent to be what X's parent was 
    y->up = x->up;

    // if X was the root 
    if (x->up == ZTNULL)
        tree->root = y;
    else {
        // Set X's parent's left or right pointer to be Y 
        if (x == x->up->left)
            x->up->left = y;
        else
            x->up->right = y;
    }

    // Put X on Y's left 
    y->left = x;

    // Set X's parent to be Y 
    x->up = y;
}

static void
s_right_rotate (ztree_t *tree, node_t *y)
{
    node_t *x;

    assert (y != ZTNULL);
    assert (y->left != ZTNULL);

    x = y->left; // set X 

    // Turn X's right subtree into Y's left subtree (move B) 
    y->left = x->right;

    // If B is not null, set it's parent to be Y 
    if (x->right != ZTNULL)
        x->right->up = y;

    // Set X's parent to be what Y's parent was 
    x->up = y->up;

    // if Y was the root 
    if (y->up == ZTNULL)
        tree->root = x;
    else {
        // Set Y's parent's left or right pointer to be X 
        if (y == y->up->left)
            y->up->left = x;
        else
            y->up->right = x;
    }
    // Put Y on X's right 
    x->right = y;

    // Set Y's parent to be X 
    y->up = x;
}


//  Having added a red node, we must now walk back up the tree balancing
//  it, by a series of rotations and changing of colours

static void s_insert_color (ztree_t *tree, node_t *x)
{
    //  While we are not at the top and our parent node is red
    //  N.B. Since the root node is guaranteed black, then we
    //  are also going to stop if we are the child of the root
    
    while (x != tree->root && x->up->colour == red) {
        // if our parent is on the left side of our grandparent 
        if (x->up == x->up->up->left) {
            // get the right side of our grandparent (uncle?) 
            node_t *y = x->up->up->right;       // uncle 
            if (y->colour == red) {
                x->up->colour = black;          // make our parent black 
                y->colour = black;              // make our uncle black 
                x->up->up->colour = red;        // make our grandparent red 
                x = x->up->up;                  // now consider grandparent 
                continue;
            }
            // if we are on the right side of our parent 
            if (x == x->up->right) {
                // Move up to our parent 
                x = x->up;
                s_left_rotate (tree, x);
            }

            x->up->colour = black;         // make our parent black 
            x->up->up->colour = red;        // make our grandparent red 
            // right rotate our grandparent 
            s_right_rotate (tree, x->up->up);
        } 
        else {
            // everything here is the same as above, but
            // exchanging left for right
            node_t *y = x->up->up->left;
            if (y->colour == red) {
                x->up->colour = black;
                y->colour = black;
                x->up->up->colour = red;
                x = x->up->up;
                continue;
            }
            if (x == x->up->left) {
                x = x->up;
                s_right_rotate (tree, x);
            }
            x->up->colour = black;
            x->up->up->colour = red;
            s_left_rotate (tree, x->up->up);
        }
    }
    tree->root->colour = black; // Set the root node black 
}

// Local function to traverse tree and optionally insert new node
enum insert_action { none, insert, replace };

static node_t *
s_traverse (ztree_t *tree, const char *key, void *value,
            enum insert_action action)
{
    ztree_compare_fn *compare = tree->compare;
    node_t *x, *y;

    x = tree->root;
    y = ZTNULL;
    while (x != ZTNULL) {
        y = x;

        int cmp = compare (key, x->key);
        if (cmp < 0)
            x = x->left;
        else 
        if (cmp > 0)
            x = x->right;
        else {
            switch (action) {
                case none:
                    return x;
                case insert:
                    return NULL;
                case replace:
                    s_free_value (tree, x);
                    x->value = value;
                    return x;
            }
        }
    }
    if (action == none)
        return NULL;        // not found

    node_t *z = zmalloc (sizeof (node_t));
    if (!z)
        return NULL; // no memory

    if (tree->autofree)
        value = strdup ((char *) value);

    z->key = strdup (key);
    z->value = value;
    z->left = z->right = ZTNULL;
    z->up = y;
    z->colour = red;

    if (y == ZTNULL)
        tree->root = z;
    else {
        if (compare (z->key, y->key) < 0)
            y->left = z;
        else
            y->right = z;
    }
    s_insert_color (tree, z);
    return z;
}

//  --------------------------------------------------------------------------
//  Insert node into tree with specified key and value
//  If key is already present returns -1 and leaves existing node unchanged
//  Returns 0 on success.

int
ztree_insert (ztree_t *self, const char *key, void *value)
{
    assert (self);
    assert (key);

    return s_traverse (self, key, value, insert) ? 0 : -1;
}

//  --------------------------------------------------------------------------
//  Update node into tree with specified key and value.
//  If key is already present, destroys old node and inserts new one.
//  Use free_fn method to ensure deallocator is properly called on value.

void
ztree_update (ztree_t *self, const char *key, void *value)
{
    assert (self);
    assert (key);

    s_traverse (self, key, value, replace);
}

// Helper to restore the reb-black properties after a delete
static void
s_remove_color (ztree_t *self, node_t *x)
{

    while (x != self->root && x->colour == black) {
        if (x == x->up->left) {
            node_t *w = x->up->right;
            if (w->colour == red) {
                w->colour = black;
                x->up->colour = red;
                s_left_rotate (self, x->up);
                w = x->up->right;
            }

            if (w->left->colour == black && w->right->colour == black) {
                w->colour = red;
                x = x->up;
            } 
            else {
                if (w->right->colour == black) {
                    w->left->colour = black;
                    w->colour = red;
                    s_right_rotate (self, w);
                    w = x->up->right;
                }
                w->colour = x->up->colour;
                x->up->colour = black;
                w->right->colour = black;
                s_left_rotate (self, x->up);
                x = self->root;
            }
        } 
        else {
            node_t *w = x->up->left;
            if (w->colour == red) {
                w->colour = black;
                x->up->colour = red;
                s_right_rotate (self, x->up);
                w = x->up->left;
            }
            if (w->left->colour == black && w->right->colour == black) {
                w->colour = red;
                x = x->up;
            } 
            else {
                if (w->left->colour == black) {
                    w->right->colour = black;
                    w->colour = red;
                    s_left_rotate (self, w);
                    w = x->up->left;
                }
                w->colour = x->up->colour;
                x->up->colour = black;
                w->left->colour = black;
                s_right_rotate (self, x->up);
                x = self->root;
            }
        }
    }
    x->colour = black;
}

// Helper function return a pointer to the smallest key greater than x

static node_t *
s_successor (const node_t *x)
{
    node_t *y;

    if (x->right != ZTNULL) {
        // If right is not NULL then go right one and then keep going left
        // until we find a node with no left pointer.
        for (y = x->right; y->left != ZTNULL; y = y->left)
            ;
    } 
    else {
        // Go up the tree until we get to a node that is on the
        // left of its parent (or the root) and then return the  parent.
        y = x->up;
        while (y != ZTNULL && x == y->right) {
            x = y;
            y = y->up;
        }
    }
    return y;
}

// Helper function to delete node from tree
static void
s_delete (ztree_t *self, node_t *z)
{
    node_t *x, *y;

    if (z->left == ZTNULL || z->right == ZTNULL)
        y = z;
    else
        y = s_successor (z);

    if (y->left != ZTNULL)
        x = y->left;
    else
        x = y->right;

    x->up = y->up;
    if (y->up == ZTNULL)
        self->root = x;
    else {
        if (y == y->up->left)
            y->up->left = x;
        else
            y->up->right = x;
    }
    if (y != z) {
        free (z->key);
        s_free_value (self, z);

        z->key = y->key;
        z->value = y->value;
    } 
    else {
        free (y->key);
        s_free_value (self, y);
    }
    if (y->colour == black)
        s_remove_color (self, x);

    free (y);
}


//  --------------------------------------------------------------------------
//  Remove a node  specified by key from the tree. If there was no such
//  node, this function does nothing.

void
ztree_delete (ztree_t *self, const char *key)
{
    assert (self);
    assert (key);

    node_t *x = s_traverse (self, key, NULL, none);
    if (x)
        s_delete (self, x);
}


//  --------------------------------------------------------------------------
//  Look for node in tree and return its value, or NULL

void *
ztree_lookup (ztree_t *self, const char *key)
{
    assert (self);
    assert (key);

    node_t *n = s_traverse (self, key, NULL, none);
    return n ? n->value : NULL;
}

//  --------------------------------------------------------------------------
//  Set a free function for the specified tree node. When the ndoe is
//  destroyed, the free function, if any, is called on that node.
//  Use this when tree nodes are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the value, or NULL if there is no such node.

void *
ztree_freefn (ztree_t *self, const char *key, ztree_free_fn *free_fn)
{
    assert (self);
    assert (key);

    node_t *x = ztree_lookup (self, key);
    if (x) {
        x->free_fn = free_fn;
        return x->value;
    } 
    else
        return NULL;
}

// Local helper function
// Recursively walk tree, stopping if callback returns non-zero

static int s_walk ( node_t *x, ztree_walk_fn *callback, void *argument)
{
    if (x == ZTNULL)
        return 0;        // empty

    int ret = s_walk (x->left, callback, argument);
    if (ret == 0)
        ret = (*callback) (x->key, x->value, argument);
    if (ret == 0)
        ret = s_walk (x->right, callback, argument);
    return ret;
}

//  --------------------------------------------------------------------------
//  Apply function to each node in the tree. Nodes are iterated in order
//  defined by comparison function.  Stops if callback function returns
//  non-zero and returns final return code from callback function
// (zero = success).

int
ztree_walk (ztree_t *self, ztree_walk_fn *callback, void *argument)
{
    assert (self);
    return s_walk (self->root, callback, argument);
}

// Local helper to count nodes
static int s_count (const char *key, void *value, void *arg)
{
    *(size_t *) arg += 1;
    return 0;
}


size_t
ztree_size (ztree_t *self)
{
    assert (self);
    size_t nodes = 0;
    s_walk (self->root, s_count, &nodes);
    return nodes;
}

// Local helper to insert key into list
static int s_addkey (const char *key, void *value, void *arg)
{
    zlist_t *keys = arg;
    zlist_append (keys, (char *) key);
    return 0;
}

//  -------------------------------------------------------------------------
//  Return keys for nodes in tree

zlist_t *
ztree_keys (ztree_t *self)
{
    assert (self);
    zlist_t *keys = zlist_new ();
    zlist_autofree (keys);
    s_walk (self->root, s_addkey, keys);
    return keys;
}

// Local helper to duplicate tree
static int s_dupkey (const char *key, void *value, void *arg)
{
    ztree_t *copy = arg;
    ztree_insert (copy, key, value);
    return 0;
}

//  --------------------------------------------------------------------------
//  Make copy of tree
//  Does not copy values themselves. Could be faster since in order
//  inserts cause lots of rebalancing.
//  NOTE: only works with values that are strings
//  since there's no other way to know how to duplicate the value.

ztree_t *
ztree_dup (ztree_t *self)
{
    if (!self)
        return NULL;

    ztree_t *copy = ztree_new (self->compare);
    if (copy)
        ztree_walk (self, s_dupkey, copy);

    return copy;
}

// Local helper function to print node to file

static int s_print (const char *key, void *value, void *arg)
{
    FILE *handle = arg;
    fprintf (handle, "%s=%s\n", key, (char *) value);
    return 0;
}

//  --------------------------------------------------------------------------
//  Save tree to a text file in name=value format
//  Hash values must be printable strings; keys may not contain '=' character
//  Returns 0 if OK, else -1 if a file error occurred

int
ztree_save (ztree_t *self, const char *filename)
{
    assert (self);
    FILE *handle = fopen (filename, "w");
    if (!handle)
        return -1;              //  Failed to create file

    s_walk (self->root, s_print, handle);
    fclose (handle);
    return 0;
}

//  --------------------------------------------------------------------------
//  Load hash table from a text file in name=value format; hash table must
//  already exist. Hash values must printable strings; keys may not contain
//  '=' character. Returns 0 if OK, else -1 if a file was not readable.

int
ztree_load (ztree_t *self, const char *filename)
{
    assert (self);
    ztree_autofree (self);

    FILE *handle = fopen (filename, "r");
    if (!handle)
        return -1;              //  Failed to create file

    char buffer [1024];
    while (fgets (buffer, 1024, handle)) {
        //  Buffer will end in newline, which we don't want
        if (buffer [strlen (buffer) - 1] == '\n')
            buffer [strlen (buffer) - 1] = 0;
        //  Split at equals, if any
        char *equals = strchr (buffer, '=');
        if (!equals)
            break;              //  Some error, stop parsing it
        *equals++ = 0;
        ztree_update (self, buffer, equals);
    }
    fclose (handle);
    return 0;
}

//  --------------------------------------------------------------------------
//  Set tree for automatic value destruction

void
ztree_autofree (ztree_t *self)
{
    assert (self);
    self->autofree = true;
}

static int test_walk (const char *key, void *value, void *arg)
{
    assert (NULL != ztree_lookup ((ztree_t *) arg, key));
    return 0;
}

static int test_walk_error (const char *key, void *value, void *arg)
{
    return -1;
}

void
ztree_test (int verbose)
{
    printf (" * ztree: ");

    //  @selftest
    ztree_t *tree = ztree_new (strcmp);
    assert (tree);
    assert (ztree_size (tree) == 0);

    assert (ztree_lookup (tree, "NOTHING") == NULL);

    //  Insert some nodes
    int rc;
    rc = ztree_insert (tree, "DEADBEEF", "dead beef");
    assert (rc == 0);
    rc = ztree_insert (tree, "ABADCAFE", "a bad cafe");
    assert (rc == 0);
    rc = ztree_insert (tree, "C0DEDBAD", "coded bad");
    assert (rc == 0);
    rc = ztree_insert (tree, "DEADF00D", "dead food");
    assert (rc == 0);
    assert (ztree_size (tree) == 4);

    //  Look for existing nodes
    char *value;
    value = (char *) ztree_lookup (tree, "DEADBEEF");
    assert (streq (value, "dead beef"));
    value = (char *) ztree_lookup (tree, "ABADCAFE");
    assert (streq (value, "a bad cafe"));
    value = (char *) ztree_lookup (tree, "C0DEDBAD");
    assert (streq (value, "coded bad"));
    value = (char *) ztree_lookup (tree, "DEADF00D");
    assert (streq (value, "dead food"));

    //  Look for non-existent nodes
    value = (char *) ztree_lookup (tree, "foo");
    assert (value == NULL);

    //  Try to insert duplicate nodes
    rc = ztree_insert (tree, "DEADBEEF", "foo");
    assert (rc == -1);
    value = (char *) ztree_lookup (tree, "DEADBEEF");
    assert (streq (value, "dead beef"));

    //  Test keys method
    zlist_t *keys = ztree_keys (tree);
    assert (zlist_size (keys) == 4);

    // Test that keys are in order
    void *key, *pred;
    pred = zlist_first (keys);
    assert (pred);
    while ((key = zlist_next (keys))) {
        assert (strcmp (key, pred) > 0);
        pred = key;
    }
    zlist_destroy (&keys);

    //  Test dup method
    ztree_t *copy = ztree_dup (tree);
    assert (ztree_size (copy) == ztree_size (tree));
    value = (char *) ztree_lookup (copy, "DEADF00D");
    assert (value);
    assert (streq (value, "dead food"));
    ztree_destroy (&copy);

    // Test walk
    assert (0 == ztree_walk (tree, test_walk, tree));
    assert (-1 == ztree_walk (tree, test_walk_error, tree));

    //  Test save and load
    ztree_save (tree, ".cache");
    copy = ztree_new (strcasecmp);
    ztree_load (copy, ".cache");
    assert (ztree_size (copy) == ztree_size (tree));
    value = (char *) ztree_lookup (copy, "DEADBEEF");
    assert (value);
    assert (streq (value, "dead beef"));
    ztree_destroy (&copy);
#if (defined (WIN32))
    DeleteFile (".cache");
#else
    unlink (".cache");
#endif

    //  Delete some nodes
    assert (ztree_size (tree) == 4);
    ztree_delete (tree, "DEADF00D");
    value = (char *) ztree_lookup (tree, "DEADF00D");
    assert (value == NULL);
    assert (ztree_size (tree) == 3);

    ztree_delete (tree, "C0DEDBAD");
    value = (char *) ztree_lookup (tree, "C0DEDBAD");
    assert (value == NULL);
    assert (ztree_size (tree) == 2);

    //  Check that the queue is robust against random usage
    struct {
        char name [100];
        bool exists;
    } testset [200];
    memset (testset, 0, sizeof (testset));
    int testmax = 200, testnbr, iteration;

    srandom ((unsigned) time (NULL));
    for (iteration = 0; iteration < 25000; iteration++) {
        testnbr = randof (testmax);
        if (testset [testnbr].exists) {
            value = (char *) ztree_lookup (tree, testset [testnbr].name);
            assert (value);
            ztree_delete (tree, testset [testnbr].name);
            testset [testnbr].exists = false;
        }
        else {
            sprintf (testset [testnbr].name, "%x-%x", rand (), rand ());
            if (ztree_insert (tree, testset [testnbr].name, "") == 0)
                testset [testnbr].exists = true;
        }
    }

    //  Test 10K lookups
    for (iteration = 0; iteration < 10000; iteration++)
        value = (char *) ztree_lookup (tree, "DEADBEEFABADCAFE");

    //  Destructor should be safe to call twice
    ztree_destroy (&tree);
    ztree_destroy (&tree);
    assert (tree == NULL);
    //  @end

    printf ("OK\n");
}
