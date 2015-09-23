#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Ztrie.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_destroy (JNIEnv *env, jobject thisObj) {
}

//  Print properties of the ztrie object.
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_print (JNIEnv *env, jobject thisObj) {
}

//  Inserts a new route into the tree and attaches the data. Returns -1     
//  if the route already exists, otherwise 0. This method takes ownership of
//  the provided data if a destroy_data_fn is provided.                     
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_insertRoute (JNIEnv *env, jobject thisObj) {
}

//  Removes a route from the trie and destroys its data. Returns -1 if the
//  route does not exists, otherwise 0.                                   
//  the start of the list call zlist_first (). Advances the cursor.       
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_removeRoute (JNIEnv *env, jobject thisObj) {
}

//  Returns true if the path matches a route in the tree, otherwise false.
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_matches (JNIEnv *env, jobject thisObj) {
}

//  Returns the data of a matched route from last ztrie_matches. If the path
//  did not match, returns NULL. Do not delete the data as it's owned by    
//  ztrie.                                                                  
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_hitData (JNIEnv *env, jobject thisObj) {
}

//  Returns the count of parameters that a matched route has.
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_hitParameterCount (JNIEnv *env, jobject thisObj) {
}

//  Returns the parameters of a matched route with named regexes from last   
//  ztrie_matches. If the path did not match or the route did not contain any
//  named regexes, returns NULL.                                             
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_hitParameters (JNIEnv *env, jobject thisObj) {
}

//  Returns the asterisk matched part of a route, if there has been no match
//  or no asterisk match, returns NULL.                                     
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_hitAsteriskMatch (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Ztrie_test (JNIEnv *env, jobject thisObj) {
}
