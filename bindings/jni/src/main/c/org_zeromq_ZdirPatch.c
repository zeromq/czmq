#include <jni.h>
#include <stdio.h>
#include "org_zeromq_ZdirPatch.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_destroy (JNIEnv *env, jobject thisObj) {
}

//  Create copy of a patch. If the patch is null, or memory was exhausted,
//  returns null.                                                         
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_dup (JNIEnv *env, jobject thisObj) {
}

//  Return patch file directory path
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_path (JNIEnv *env, jobject thisObj) {
}

//  Return patch file item
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_file (JNIEnv *env, jobject thisObj) {
}

//  Return operation
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_op (JNIEnv *env, jobject thisObj) {
}

//  Return patch virtual file path
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_vpath (JNIEnv *env, jobject thisObj) {
}

//  Calculate hash digest for file (create only)
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_digestSet (JNIEnv *env, jobject thisObj) {
}

//  Return hash digest for patch file
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_digest (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class.
JNIEXPORT void JNICALL Java_org_zeromq_ZdirPatch_test (JNIEnv *env, jobject thisObj) {
}
