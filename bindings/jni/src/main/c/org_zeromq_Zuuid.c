#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zuuid.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_destroy (JNIEnv *env, jobject thisObj) {
}

//  Print properties of the zuuid object.
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_print (JNIEnv *env, jobject thisObj) {
}

//  Create UUID object from supplied ZUUID_LEN-octet value.
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_newFrom (JNIEnv *env, jobject thisObj) {
}

//  Set UUID to new supplied ZUUID_LEN-octet value.
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_set (JNIEnv *env, jobject thisObj) {
}

//  Set UUID to new supplied string value skipping '-' and '{' '}'
//  optional delimiters. Return 0 if OK, else returns -1.         
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_setStr (JNIEnv *env, jobject thisObj) {
}

//  Return UUID binary data.
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_data (JNIEnv *env, jobject thisObj) {
}

//  Return UUID binary size
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_size (JNIEnv *env, jobject thisObj) {
}

//  Returns UUID as string
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_str (JNIEnv *env, jobject thisObj) {
}

//  Return UUID in the canonical string format: 8-4-4-4-12, in lower
//  case. Caller does not modify or free returned value. See        
//  http://en.wikipedia.org/wiki/Universally_unique_identifier      
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_strCanonical (JNIEnv *env, jobject thisObj) {
}

//  Store UUID blob in target array
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_export (JNIEnv *env, jobject thisObj) {
}

//  Check if UUID is same as supplied value
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_eq (JNIEnv *env, jobject thisObj) {
}

//  Check if UUID is different from supplied value
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_neq (JNIEnv *env, jobject thisObj) {
}

//  Make copy of UUID object; if uuid is null, or memory was exhausted,
//  returns null.                                                      
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_dup (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zuuid_test (JNIEnv *env, jobject thisObj) {
}
