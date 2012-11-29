#include "../include/czmq.h"

#define MAX_NODE_HEIGHT 64

//height represents the forward non null characters
//not the actual height of the node
//the actual height of the node can be discovered through searching

struct _zsklnode_t
{
    int height;
    struct _zsklnode_t **next;
    uint64_t key;
};


int
comp_node_t (zsklnode_t * first, zsklnode_t * second)
{
    if (first->key > second->key) {
        return 1;
    }
    else {

        if (first->key < second->key) {
            return -1;
        }
        else {
            return 0;
        }
    }

}


zsklnode_t *
node_new (uint64_t key)
{
    zsklnode_t *node = zmalloc (sizeof (zsklnode_t));
    node->key = key;

    return node;
}

void
node_destroy (zsklnode_t ** node)
{
    if ((*node)->next) {
        free ((*node)->next);
    }
    free (*node);
    node = NULL;
}





struct _zsklist_t
{
    zsklnode_t *head;
};


zsklist_t *
zsklist_new (void)
{
    zsklist_t *zsklist = malloc (sizeof (zsklist_t));
    zsklist->head = node_new (0);
    zsklist->head->height = 1;
    zsklist->head->next = zmalloc (sizeof (zsklnode_t *));
    return zsklist;
}

void
zsklist_destroy (zsklist_t ** zsklist)
{

    while ((*zsklist)->head->next[0]) {
        zsklist_delete (*zsklist, (*zsklist)->head->next[0]->key);
    }
    free ((*zsklist)->head->next);
    free ((*zsklist)->head);
    free (*zsklist);
    zsklist = NULL;

}


int
zsklist_add_ (zsklist_t * zsklist, int height, uint64_t key)
{
    assert (key > 0);
    zsklnode_t *node = node_new (key);

    zsklnode_t *prev_list[height];

    int iter = height;
    zsklnode_t *ptr = zsklist->head;
    while (iter) {
        if (!(ptr->next[iter - 1])) {
            prev_list[iter] = ptr;
            iter--;
        }
        else {
            int comp = comp_node_t (node, ptr->next[iter - 1]);
            if (comp == 1) {
                ptr = ptr->next[iter - 1];
            }
            else {
                if (comp == -1) {
                    prev_list[iter] = ptr;
                    iter--;
                }
                else {
                    node_destroy (&node);
                    return 0;
                }
            }
        }
    }

    node->height = 1;
    if ((node->height < MAX_NODE_HEIGHT) && ((rand () % 4) == 0)) {
        node->height++;
    }

    node->next = zmalloc (node->height * sizeof (zsklnode_t *));

    for (iter = 1; iter <= height; iter++) {
        node->next[iter - 1] = prev_list[iter]->next[iter - 1];
        prev_list[iter]->next[iter - 1] = node;
    }

    return 1;

}

//returns 0 if the key already exists
int
zsklist_add (zsklist_t * zsklist, uint64_t key)
{
    return zsklist_add_ (zsklist, zsklist->head->height, key);
}

int
zsklist_delete_ (zsklist_t * zsklist, int height, uint64_t key)
{

    assert (key > 0);
    zsklnode_t node;
    node.key = key;

    zsklnode_t *prev_list[height];

    int iter = height;
    zsklnode_t *ptr = zsklist->head;
    int comp = 1;
    while (iter) {
        if (!(ptr->next[iter - 1])) {
            prev_list[iter] = ptr;
            iter--;
        }
        else {
            comp = comp_node_t (&node, ptr->next[iter - 1]);
            if (comp == 1) {
                ptr = ptr->next[iter - 1];
            }
            else {
                prev_list[iter] = ptr;
                iter--;

            }
        }
    }

    if (!comp) {
        ptr = ptr->next[0];

        for (iter = 1; iter <= height; iter++) {
            prev_list[iter]->next[iter - 1] = ptr->next[iter - 1];
        }
        node_destroy (&ptr);


        return 1;

    }
    else {
        return 0;

    }
}


int
zsklist_delete (zsklist_t * zsklist, uint64_t key)
{
    return zsklist_delete_ (zsklist, zsklist->head->height, key);
}




uint64_t
zsklist_search (zsklist_t * zsklist, uint64_t key)
{
    if (zsklist_lsearch (zsklist, key, zsklist->head, NULL)) {
        return key;
    }
    else {
        return 0;
    }

}

zsklnode_t *
zsklist_lsearch (zsklist_t * t, uint64_t key,
                 zsklnode_t * rlimit, zsklnode_t * llimit)
{
    zsklnode_t node;
    node.key = key;

    int iter = rlimit->height;
    if (llimit) {
        if (llimit->height < iter) {
            iter = llimit->height;
        }
    }
    zsklnode_t *ptr = rlimit;
    while (iter) {
        if (!(ptr->next[iter - 1])) {
            iter--;
        }
        else {
            int comp = comp_node_t (&node, ptr->next[iter - 1]);
            if (comp == 1) {
                ptr = ptr->next[iter - 1];
            }
            else {
                if (comp == -1) {
                    iter--;
                }
                else {
                    return ptr;
                }
            }
        }
    }

    return NULL;
}


void
zsklist_test (int verbose)
{
    printf (" * zsklist: ");
    zsklist_t *list = zsklist_new ();

    uint64_t key[100];
    int iter;
    for (iter = 0; iter < 100; iter++) {
        key[iter] = rand () % 10000 + 1;
        zsklist_add (list, key[iter]);
    }
    if (verbose) {
        printf ("\nkey:%lu , search result:%lu", key[77],
                zsklist_search (list, key[77]));
        printf ("\nkey:%lu , search result:%lu", key[33],
                zsklist_search (list, key[33]));
        printf ("\nkey:%lu , search result:%lu", key[66],
                zsklist_search (list, key[66]));
        printf ("\nkey:%lu , search result:%lu", key[22],
                zsklist_search (list, key[22]));
    }
    assert (key[77] == zsklist_search (list, key[77]));
    assert (key[33] == zsklist_search (list, key[33]));
    assert (key[66] == zsklist_search (list, key[66]));
    assert (key[22] == zsklist_search (list, key[22]));

    assert (zsklist_delete (list, key[77]));
    assert (0 == zsklist_search (list, key[77]));

    assert (zsklist_delete (list, key[33]));
    assert (0 == zsklist_search (list, key[33]));

    zsklist_destroy (&list);

    printf ("OK\n");
}
