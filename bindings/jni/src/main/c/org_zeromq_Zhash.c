#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zhash.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zhash_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zhash_destroy (JNIEnv *env, jobject thisObj) {
}

//  Insert item into hash table with specified key and item.               
//  If key is already present returns -1 and leaves existing item unchanged
//  Returns 0 on success.                                                  
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_insert (JNIEnv *env, jobject thisObj) {
}

//  Update item into hash table with specified key and item.            
//  If key is already present, destroys old item and inserts new one.   
//  Use free_fn method to ensure deallocator is properly called on item.
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_update (JNIEnv *env, jobject thisObj) {
}

//  Remove an item specified by key from the hash table. If there was no such
//  item, this function does nothing.                                        
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_delete (JNIEnv *env, jobject thisObj) {
}

//  Return the item at the specified key, or null
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_lookup (JNIEnv *env, jobject thisObj) {
}

//  Reindexes an item from an old key to a new key. If there was no such
//  item, does nothing. Returns 0 if successful, else -1.               
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_rename (JNIEnv *env, jobject thisObj) {
}

//  Set a free function for the specified hash table item. When the item is
//  destroyed, the free function, if any, is called on that item.          
//  Use this when hash items are dynamically allocated, to ensure that     
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn. 
//  Returns the item, or NULL if there is no such item.                    
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_freefn (JNIEnv *env, jobject thisObj) {
}

//  Return the number of keys/items in the hash table
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_size (JNIEnv *env, jobject thisObj) {
}

//  Make copy of hash table; if supplied table is null, returns null.    
//  Does not copy items themselves. Rebuilds new table so may be slow on 
//  very large tables. NOTE: only works with item values that are strings
//  since there's no other way to know how to duplicate the item value.  
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_dup (JNIEnv *env, jobject thisObj) {
}

//  Return keys for items in table
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_keys (JNIEnv *env, jobject thisObj) {
}

//  Simple iterator; returns first item in hash table, in no given order, 
//  or NULL if the table is empty. This method is simpler to use than the 
//  foreach() method, which is deprecated. To access the key for this item
//  use zhash_cursor(). NOTE: do NOT modify the table while iterating.    
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_first (JNIEnv *env, jobject thisObj) {
}

//  Simple iterator; returns next item in hash table, in no given order, 
//  or NULL if the last item was already returned. Use this together with
//  zhash_first() to process all items in a hash table. If you need the  
//  items in sorted order, use zhash_keys() and then zlist_sort(). To    
//  access the key for this item use zhash_cursor(). NOTE: do NOT modify 
//  the table while iterating.                                           
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_next (JNIEnv *env, jobject thisObj) {
}

//  After a successful first/next method, returns the key for the item that
//  was returned. This is a constant string that you may not modify or     
//  deallocate, and which lasts as long as the item in the hash. After an  
//  unsuccessful first/next, returns NULL.                                 
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_cursor (JNIEnv *env, jobject thisObj) {
}

//  Add a comment to hash table before saving to disk. You can add as many   
//  comment lines as you like. These comment lines are discarded when loading
//  the file. If you use a null format, all comments are deleted.            
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_comment (JNIEnv *env, jobject thisObj) {
}

//  Serialize hash table to a binary frame that can be sent in a message.
//  The packed format is compatible with the 'dictionary' type defined in
//  http://rfc.zeromq.org/spec:35/FILEMQ, and implemented by zproto:     
//                                                                       
//     ; A list of name/value pairs                                      
//     dictionary      = dict-count *( dict-name dict-value )            
//     dict-count      = number-4                                        
//     dict-value      = longstr                                         
//     dict-name       = string                                          
//                                                                       
//     ; Strings are always length + text contents                       
//     longstr         = number-4 *VCHAR                                 
//     string          = number-1 *VCHAR                                 
//                                                                       
//     ; Numbers are unsigned integers in network byte order             
//     number-1        = 1OCTET                                          
//     number-4        = 4OCTET                                          
//                                                                       
//  Comments are not included in the packed data. Item values MUST be    
//  strings.                                                             
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_pack (JNIEnv *env, jobject thisObj) {
}

//  Unpack binary frame into a new hash table. Packed data must follow format
//  defined by zhash_pack. Hash table is set to autofree. An empty frame     
//  unpacks to an empty hash table.                                          
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_unpack (JNIEnv *env, jobject thisObj) {
}

//  Save hash table to a text file in name=value format. Hash values must be
//  printable strings; keys may not contain '=' character. Returns 0 if OK, 
//  else -1 if a file error occurred.                                       
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_save (JNIEnv *env, jobject thisObj) {
}

//  Load hash table from a text file in name=value format; hash table must 
//  already exist. Hash values must printable strings; keys may not contain
//  '=' character. Returns 0 if OK, else -1 if a file was not readable.    
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_load (JNIEnv *env, jobject thisObj) {
}

//  When a hash table was loaded from a file by zhash_load, this method will
//  reload the file if it has been modified since, and is "stable", i.e. not
//  still changing. Returns 0 if OK, -1 if there was an error reloading the 
//  file.                                                                   
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_refresh (JNIEnv *env, jobject thisObj) {
}

//  Set hash for automatic value destruction
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_autofree (JNIEnv *env, jobject thisObj) {
}

//  DEPRECATED as clumsy -- use zhash_first/_next instead                  
//  Apply function to each item in the hash table. Items are iterated in no
//  defined order. Stops if callback function returns non-zero and returns 
//  final return code from callback function (zero = success).             
//  Callback function for zhash_foreach method                             
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_foreach (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zhash_test (JNIEnv *env, jobject thisObj) {
}
