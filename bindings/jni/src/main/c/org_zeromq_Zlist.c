#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zlist.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zlist_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zlist_destroy (JNIEnv *env, jobject thisObj) {
}

//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.  
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_first (JNIEnv *env, jobject thisObj) {
}

//  Return the next item. If the list is empty, returns NULL. To move to
//  the start of the list call zlist_first (). Advances the cursor.     
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_next (JNIEnv *env, jobject thisObj) {
}

//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.  
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_last (JNIEnv *env, jobject thisObj) {
}

//  Return first item in the list, or null, leaves the cursor
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_head (JNIEnv *env, jobject thisObj) {
}

//  Return last item in the list, or null, leaves the cursor
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_tail (JNIEnv *env, jobject thisObj) {
}

//  Return the current item of list. If the list is empty, returns NULL.     
//  Leaves cursor pointing at the current item, or NULL if the list is empty.
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_item (JNIEnv *env, jobject thisObj) {
}

//  Append an item to the end of the list, return 0 if OK or -1 if this  
//  failed for some reason (out of memory). Note that if a duplicator has
//  been set, this method will also duplicate the item.                  
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_append (JNIEnv *env, jobject thisObj) {
}

//  Push an item to the start of the list, return 0 if OK or -1 if this  
//  failed for some reason (out of memory). Note that if a duplicator has
//  been set, this method will also duplicate the item.                  
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_push (JNIEnv *env, jobject thisObj) {
}

//  Pop the item off the start of the list, if any
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_pop (JNIEnv *env, jobject thisObj) {
}

//  Checks if an item already is present. Uses compare method to determine if 
//  items are equal. If the compare method is NULL the check will only compare
//  pointers. Returns true if item is present else false.                     
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_exists (JNIEnv *env, jobject thisObj) {
}

//  Remove the specified item from the list if present
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_remove (JNIEnv *env, jobject thisObj) {
}

//  Make a copy of list. If the list has autofree set, the copied list will  
//  duplicate all items, which must be strings. Otherwise, the list will hold
//  pointers back to the items in the original list. If list is null, returns
//  NULL.                                                                    
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_dup (JNIEnv *env, jobject thisObj) {
}

//  Purge all items from list
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_purge (JNIEnv *env, jobject thisObj) {
}

//  Return number of items in the list
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_size (JNIEnv *env, jobject thisObj) {
}

//  Sort the list by ascending key value using a straight ASCII comparison.
//  The sort is not stable, so may reorder items with the same keys.       
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_sort (JNIEnv *env, jobject thisObj) {
}

//  Set list for automatic item destruction; item values MUST be strings. 
//  By default a list item refers to a value held elsewhere. When you set 
//  this, each time you append or push a list item, zlist will take a copy
//  of the string value. Then, when you destroy the list, it will free all
//  item values automatically. If you use any other technique to allocate 
//  list values, you must free them explicitly before destroying the list.
//  The usual technique is to pop list items and destroy them, until the  
//  list is empty.                                                        
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_autofree (JNIEnv *env, jobject thisObj) {
}

//  Sets a compare function for this list. The function compares two items.
//  It returns an integer less than, equal to, or greater than zero if the 
//  first item is found, respectively, to be less than, to match, or be    
//  greater than the second item.                                          
//  This function is used for sorting, removal and exists checking.        
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_comparefn (JNIEnv *env, jobject thisObj) {
}

//  Set a free function for the specified list item. When the item is     
//  destroyed, the free function, if any, is called on that item.         
//  Use this when list items are dynamically allocated, to ensure that    
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.                   
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_freefn (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zlist_test (JNIEnv *env, jobject thisObj) {
}
