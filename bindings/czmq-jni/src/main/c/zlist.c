#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "czmq.h"
#include "../../native/include/org_zeromq_czmq_ZList.h"

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZList__1_1init (JNIEnv * env, jclass c) {
    zlist_t *list = zlist_new ();
    if (list) {
        return (jlong) list;
    }
    return -1;
}

JNIEXPORT void JNICALL
Java_org_zeromq_czmq_ZList__1_1destroy (JNIEnv *env, jclass c, jlong ptr) {
    zlist_t *list = (zlist_t *) ptr;
    zlist_destroy (&list);
}

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZList__1_1size (JNIEnv *env, jclass c, jlong ptr) {
    zlist_t *list = (zlist_t *) ptr;
    return zlist_size (list);
}
