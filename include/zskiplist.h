#ifndef __ZSKIPLIST_H_INCLUDED__
#define __ZSKIPLIST_H_INCLUDED__


#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _zsklnode_t zsklnode_t;



    typedef struct _zsklist_t zsklist_t;

    zsklist_t *zsklist_new (void);

    int zsklist_add (zsklist_t * zsklist, uint64_t key);

    int zsklist_delete (zsklist_t * zsklist, uint64_t key);

    uint64_t zsklist_search (zsklist_t * zsklist, uint64_t key);

    zsklnode_t *zsklist_lsearch (zsklist_t * t, uint64_t key,
                                zsklnode_t * rlimit, zsklnode_t * llimit);



    void zsklist_test (int verbose);



#ifdef __cplusplus
}
#endif





#endif
