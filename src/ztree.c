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

    Derived from Emin Martianan's Red Black which is licensed for free use.

    http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html
*/

/*
@header
    Red black tree container
@discuss
@end
*/

#include "../include/czmq.h"

// Tree node, used internally

typedef struct _node_t {
    char *key;                  //  User's key
    void *value;                //      and datum
    bool  red;			// if !red then node is black
    struct _node_t *left, *right, *parent;
    zhash_free_fn *free_fn;     //  Value free function if any
} node_t;

struct _ztree_t {
    ztree_compare_fn *compare;  //  Comparison function
    // A sentinel is used for root and nil.
    // nil points to anode which should always be black but has
    // aribtrary children and parent and no key or value.
    // The point of using these sentinels is so that the root
    // and nil nodes do not require special cases in the code
    node_t root;                //  Red-black tree root
    node_t nil;			//  See above
    bool autofree;              //  If true, free value in destructor
};

//  --------------------------------------------------------------------------
//  Tree constructor

ztree_t *
ztree_new (ztree_compare_fn *compare)
{
    assert (compare);

    ztree_t *self = zmalloc (sizeof (ztree_t));
    if (!self)
	return NULL;

    self->compare = compare;

    node_t *nil = &self->nil;
    nil->parent = nil->left = nil->right = nil;
    nil->red = false;

    node_t *root = &self->root;
    root->parent = root->left = root->right = nil;
    root->red = false;

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
    if (x == &self->nil)
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
        s_destroy (self, self->root.left);
        free (self);
        *self_p = NULL;
    }
}

// Rotates as described in _Introduction_To_Algorithms by
// Cormen, Leiserson, Rivest (Chapter 14).  Basically this
// makes the parent of x be to the left of x, x the parent of
// its parent before the rotation and fixes other pointers accordingly.

static void
s_left_rotate (ztree_t *tree, node_t *x)
{
    node_t *y;
    node_t *nil = &tree->nil;

    //  I originally wrote this function to use the sentinel for
    //  nil to avoid checking for nil.  However this introduces a
    //  very subtle bug because sometimes this function modifies
    //  the parent pointer of nil.  This can be a problem if a
    //  function which calls LeftRotate also uses the nil sentinel
    //  and expects the nil sentinel's parent pointer to be unchanged
    //  after calling this function.  For example, when RBDeleteFixUP
    //  calls LeftRotate it expects the parent pointer of nil to be
    //  unchanged.

    y = x->right;
    x->right = y->left;

    if (y->left != nil)
	y->left->parent=x;

    y->parent = x->parent;

    // instead of checking if x->parent is the root as in the book, we
    // count on the root sentinel to implicitly take care of this case
    if( x == x->parent->left)
	x->parent->left = y;
    else
	x->parent->right = y;

    y->left = x;
    x->parent = y;

    assert(!tree->nil.red);
}

static void
s_right_rotate (ztree_t *tree, node_t *y)
{
    node_t *x;
    node_t *nil = &tree->nil;

    x = y->left;
    y->left = x->right;

    if (nil != x->right)
	x->right->parent = y;

    // instead of checking if x->parent is the root as in the book,
    // we count on the root sentinel to implicitly take care of this case
    x->parent = y->parent;
    if (y == y->parent->left)
	y->parent->left = x;
    else
	y->parent->right = x;

    x->right = y;
    y->parent = x;

  assert(!tree->nil.red);
}

// Inserts z into the tree as if it were a regular binary tree
// using the algorithm described in _Introduction_To_Algorithms_
// by Cormen et al.
// Returns existing node if key is a duplicate

static node_t *s_insert_node (ztree_t *tree, node_t *z)
{
    node_t *root = &tree->root;
    node_t *nil = &tree->nil;

    z->left = z->right = nil;

    node_t *y = root;
    node_t *x = root->left;
    while (x != nil) {
	y = x;
	int cmp = tree->compare (x->key, z->key);

	// do not allow duplicate keys
	if (cmp == 0)
	    return x;

	if (cmp > 0) // x.key > z.key
	    x = x->left;
	else // x,key <= z.key
	    x = x->right;
    }

    z->parent = y;
    if ( y == root ||
	 tree->compare (y->key, z->key) > 0) // y.key > z.key
	y->left = z;
    else
	y->right = z;

    assert(!tree->nil.red);
    return NULL;
}

// Recolor the binary tree after insertion of node x

static void s_insert_recolor (ztree_t *tree, node_t *x)
{
    while (x->parent->red) { // use sentinel instead of checking for root
	if (x->parent == x->parent->parent->left) {
	    node_t *y = x->parent->parent->right;
	    if (y->red) {
		x->parent->red = false;
		y->red = false;
		x->parent->parent->red = true;
		x = x->parent->parent;
	    } else {
		if (x == x->parent->right) {
		    x = x->parent;
		    s_left_rotate (tree, x);
		}

		x->parent->red = false;
		x->parent->parent->red = true;
		s_right_rotate (tree, x->parent->parent);
	    }
	} else { //case for x->parent == x->parent->parent->right
	    node_t *y = x->parent->parent->left;
	    if (y->red) {
		x->parent->red = false;
		y->red = false;
		x->parent->parent->red = true;
		x = x->parent->parent;
	    } else {
		if (x == x->parent->left) {
		    x = x->parent;
		    s_right_rotate (tree,x);
		}
		x->parent->red = false;
		x->parent->parent->red = true;
		s_left_rotate (tree,x->parent->parent);
	    }
	}
    }
    tree->root.left->red = false;

    assert(!tree->nil.red);
    assert(!tree->root.red);
}

// returns the a node with key equal to q.

static node_t *
s_lookup (ztree_t *tree, const char *q)
{
    node_t *nil = &tree->nil;
    node_t *x = tree->root.left;

    while (x != nil) {
	    int cmp = tree->compare (x->key, q);
	    if (cmp == 0)
		    break;
	    x = cmp > 0 ? x->left : x->right;
    }

    return x;
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

    node_t *x = zmalloc (sizeof (node_t));
    if (!x)
        return -1;

    //  If necessary, take duplicate of item (string) value
    if (self->autofree)
        value = strdup ((char *) value);

    x->key = strdup (key);
    x->value = value;
    x->red = true;

    // unwind if duplicate node
    if (s_insert_node (self, x)) {
	free (x->key);
	if (self->autofree)
	    free (value);
	free (x);
	return -1;
    }

    s_insert_recolor (self, x);
    return 0;
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

    node_t *x = zmalloc (sizeof (node_t));
    if (!x)
        return;

    //  If necessary, take duplicate of item (string) value
    if (self->autofree)
            value = strdup ((char *) value);

    x->key = strdup (key);
    x->value = value;
    x->red = true;

    node_t *y = s_insert_node (self, x);
    if (y) {
	s_free_value (self, y);
	y->value = value;
	free (x->key);
	free (x);
    } else {
	s_insert_recolor (self, x);
    }
}

// Performs rotations and changes colors to restore red-black
// properties after a node is deleted

static void s_delete_fix (ztree_t *tree, node_t *x)
{
    node_t *root = tree->root.left;

    while( (!x->red) && (root != x)) {
	if (x == x->parent->left) {
	    node_t *w = x->parent->right;
	    if (w->red) {
		w->red = false;
		x->parent->red = true;
		s_left_rotate (tree,x->parent);
		w = x->parent->right;
	    }
	    if ( (!w->right->red) && (!w->left->red) ) {
		w->red = true;
		x = x->parent;
	    } else {
		if (!w->right->red) {
		    w->left->red = false;
		    w->red = true;
		    s_right_rotate (tree,w);
		    w = x->parent->right;
		}
		w->red = x->parent->red;
		x->parent->red = false;
		w->right->red = false;
		s_left_rotate (tree,x->parent);
		x = root; // this is to exit while loop
	    }
	} else { // the code below is has left and right switched from above
	    node_t *w = x->parent->left;
	    if (w->red) {
		w->red = false;
		x->parent->red = true;
		s_right_rotate (tree,x->parent);
		w = x->parent->left;
	    }
	    if ( (!w->right->red) && (!w->left->red) ) {
		w->red = true;
		x = x->parent;
	    } else {
		if (!w->left->red) {
		    w->right->red = false;
		    w->red = true;
		    s_left_rotate (tree,w);
		    w = x->parent->left;
		}
		w->red=x->parent->red;
		x->parent->red = false;
		w->left->red = false;
		s_right_rotate (tree,x->parent);
		x = root; // this is to exit while loop
	    }
	}
    }
    x->red = false;

    assert(!tree->nil.red);
}

// Helper function return a pointer to the smallest key greater than x

static node_t *
s_successor (ztree_t *tree, const node_t *x)
{
    node_t *y;
    node_t *nil = &tree->nil;
    node_t *root = &tree->root;

    if (nil != (y = x->right)) { // assignment to y is intentional
	while (y->left != nil) { // returns the minium of the right subtree of x
	    y = y->left;
	}
    } else {
	y = x->parent;
	while (x == y->right) { // sentinel used instead of checking for nil
	    x = y;
	    y = y->parent;
	}

	if (y == root)
	    return nil;
    }

    return y;
}

// Helper function to delete node from tree
//    The algorithm from this function is from _Introduction_To_Algorithms_
static void
s_delete (ztree_t *tree, node_t *z)
{
    node_t *x, *y;
    node_t *nil = &tree->nil;
    node_t *root = &tree->root;

    if (z->left == nil || z->right == nil)
	y = z;
    else
	y = s_successor (tree, z);

    x = (y->left == nil) ? y->right : y->left;

    if (root == (x->parent = y->parent)) { // assignment of y->p to x->p is intentional
	root->left = x;
    } else {
	if (y == y->parent->left)
	    y->parent->left = x;
	else
	    y->parent->right = x;
    }

    if (y != z) {
	assert(y != &tree->nil);

	// y is the node to splice out and x is its child
	if (!y->red)
	    s_delete_fix (tree, x);

	free (z->key);
	s_free_value (tree, z);

	y->left = z->left;
	y->right = z->right;
	y->parent = z->parent;
	y->red = z->red;
	z->left->parent = z->right->parent=y;
	if (z == z->parent->left)
	    z->parent->left = y;
	else
	    z->parent->right = y;
	free(z);
    } else {
	free (y->key);
	s_free_value (tree, y);
	if (!y->red)
	    s_delete_fix (tree, x);
	free(y);
    }

    assert(!tree->nil.red);
}


//  --------------------------------------------------------------------------
//  Remove a node  specified by key from the tree. If there was no such
//  node, this function does nothing.

void
ztree_delete (ztree_t *self, const char *key)
{
    assert (self);
    assert (key);

    node_t *x = s_lookup (self, key);
    if (x != &self->nil)
        s_delete (self, x);
}


//  --------------------------------------------------------------------------
//  Look for node in tree and return its value, or NULL

void *
ztree_lookup (ztree_t *self, const char *key)
{
    assert (self);
    assert (key);

    node_t *n = s_lookup (self, key);
    return (n == &self->nil) ? NULL : n->value;
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

static int s_walk (ztree_t *tree, node_t *x,
		   ztree_walk_fn *callback, void *argument)
{
    if (x == &tree->nil)
        return 0;        // empty

    int ret = s_walk (tree, x->left, callback, argument);
    if (ret == 0)
        ret = (*callback) (x->key, x->value, argument);
    if (ret == 0)
        ret = s_walk (tree, x->right, callback, argument);
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
    return s_walk (self, self->root.left, callback, argument);
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
    s_walk (self, self->root.left, s_count, &nodes);
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
    s_walk (self, self->root.left, s_addkey, keys);
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
    if (copy) {
	ztree_autofree (copy);
        ztree_walk (self, s_dupkey, copy);
    }

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
//  Values must be printable strings; keys may not contain '=' character
//  Returns 0 if OK, else -1 if a file error occurred

int
ztree_save (ztree_t *self, const char *filename)
{
    assert (self);
    FILE *handle = fopen (filename, "w");
    if (!handle)
        return -1;              //  Failed to create file

    s_walk (self, self->root.left, s_print, handle);
    fclose (handle);
    return 0;
}

//  --------------------------------------------------------------------------
//  Load tree from a text file in name=value format; tree must
//  already exist. Values must printable strings; keys may not contain
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

    // Change value of an existing node
    ztree_update (tree, "ABADCAFE", "A Bad Cafe");
    value = (char *) ztree_lookup (tree, "ABADCAFE");
    assert (value != NULL);
    assert (streq(value, "A Bad Cafe"));

    // Update with non-existant node
    ztree_update (tree, "C0DEDEBAD", "Coded Bad");
    value = (char *) ztree_lookup (tree, "C0DEDEBAD");
    assert (value);
    assert (streq(value, "Coded Bad"));

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
