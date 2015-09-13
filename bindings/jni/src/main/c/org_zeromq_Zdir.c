#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zdir.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zdir_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zdir_destroy (JNIEnv *env, jobject thisObj) {
}

//  Return directory path
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_path (JNIEnv *env, jobject thisObj) {
}

//  Return last modification time for directory.
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_modified (JNIEnv *env, jobject thisObj) {
}

//  Return total hierarchy size, in bytes of data contained in all files
//  in the directory tree.                                              
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_cursize (JNIEnv *env, jobject thisObj) {
}

//  Return directory count
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_count (JNIEnv *env, jobject thisObj) {
}

//  Returns a sorted list of zfile objects; Each entry in the list is a pointer
//  to a zfile_t item already allocated in the zdir tree. Do not destroy the   
//  original zdir tree until you are done with this list.                      
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_list (JNIEnv *env, jobject thisObj) {
}

//  Remove directory, optionally including all files that it contains, at  
//  all levels. If force is false, will only remove the directory if empty.
//  If force is true, will remove all files and all subdirectories.        
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_remove (JNIEnv *env, jobject thisObj) {
}

//  Calculate differences between two versions of a directory tree.    
//  Returns a list of zdir_patch_t patches. Either older or newer may  
//  be null, indicating the directory is empty/absent. If alias is set,
//  generates virtual filename (minus path, plus alias).               
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_diff (JNIEnv *env, jobject thisObj) {
}

//  Return full contents of directory as a zdir_patch list.
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_resync (JNIEnv *env, jobject thisObj) {
}

//  Load directory cache; returns a hash table containing the SHA-1 digests
//  of every file in the tree. The cache is saved between runs in .cache.  
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_cache (JNIEnv *env, jobject thisObj) {
}

//  Print contents of directory to open stream
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_fprint (JNIEnv *env, jobject thisObj) {
}

//  Print contents of directory to stdout
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_print (JNIEnv *env, jobject thisObj) {
}

//  Create a new zdir_watch actor instance:                       
//                                                                
//      zactor_t *watch = zactor_new (zdir_watch, NULL);          
//                                                                
//  Destroy zdir_watch instance:                                  
//                                                                
//      zactor_destroy (&watch);                                  
//                                                                
//  Enable verbose logging of commands and activity:              
//                                                                
//      zstr_send (watch, "VERBOSE");                             
//                                                                
//  Subscribe to changes to a directory path:                     
//                                                                
//      zsock_send (watch, "ss", "SUBSCRIBE", "directory_path");  
//                                                                
//  Unsubscribe from changes to a directory path:                 
//                                                                
//      zsock_send (watch, "ss", "UNSUBSCRIBE", "directory_path");
//                                                                
//  Receive directory changes:                                    
//      zsock_recv (watch, "sp", &path, &patches);                
//                                                                
//      // Delete the received data.                              
//      free (path);                                              
//      zlist_destroy (&patches);                                 
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_watch (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class.
JNIEXPORT void JNICALL Java_org_zeromq_Zdir_test (JNIEnv *env, jobject thisObj) {
}
