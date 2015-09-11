#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Ziflist.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_destroy (JNIEnv *env, jobject thisObj) {
}

//  Print properties of the ziflist object.
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_print (JNIEnv *env, jobject thisObj) {
}

//  Reload network interfaces from system
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_reload (JNIEnv *env, jobject thisObj) {
}

//  Return the number of network interfaces on system
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_size (JNIEnv *env, jobject thisObj) {
}

//  Get first network interface, return NULL if there are none
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_first (JNIEnv *env, jobject thisObj) {
}

//  Get next network interface, return NULL if we hit the last one
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_next (JNIEnv *env, jobject thisObj) {
}

//  Return the current interface IP address as a printable string
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_address (JNIEnv *env, jobject thisObj) {
}

//  Return the current interface broadcast address as a printable string
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_broadcast (JNIEnv *env, jobject thisObj) {
}

//  Return the current interface network mask as a printable string
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_netmask (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class.
JNIEXPORT void JNICALL Java_org_zeromq_Ziflist_test (JNIEnv *env, jobject thisObj) {
}
