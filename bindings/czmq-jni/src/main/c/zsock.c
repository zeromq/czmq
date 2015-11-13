#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "czmq.h"
#include "../../native/include/org_zeromq_czmq_ZSock.h"

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZSock__1_1init (JNIEnv *env, jclass c, jint type) {
    zsock_t *sock = zsock_new (type);
    if (sock) {
        return (jlong) sock;
    }
    return -1;
}

JNIEXPORT void JNICALL
Java_org_zeromq_czmq_ZSock__1_1destroy (JNIEnv *env, jclass c, jlong ptr) {
    zsock_t *sock = (zsock_t *) ptr;
    zsock_destroy (&sock);
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZSock__1_1bind (JNIEnv *env, jclass c, jlong ptr, jstring endpoint) {
    const char *ep = (const char *) (*env)->GetStringUTFChars (env, endpoint, NULL);
    int rc = zsock_bind ((zsock_t *) ptr, "%s", ep);
    (*env)->ReleaseStringUTFChars (env, endpoint, ep);
    return rc;
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZSock__1_1unbind (JNIEnv *env, jclass c, jlong ptr, jstring endpoint) {
    const char *ep = (const char *) (*env)->GetStringUTFChars (env, endpoint, NULL);
    int rc = zsock_unbind ((zsock_t *) ptr, "%s", ep);
    (*env)->ReleaseStringUTFChars (env, endpoint, ep);
    return rc;
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZSock__1_1connect (JNIEnv *env, jclass c, jlong ptr, jstring endpoint) {
    const char *ep = (const char *) (*env)->GetStringUTFChars (env, endpoint, NULL);
    int rc = zsock_connect ((zsock_t *) ptr, "%s", ep);
    (*env)->ReleaseStringUTFChars (env, endpoint, ep);
    return rc;
}

JNIEXPORT jint JNICALL
Java_org_zeromq_czmq_ZSock__1_1disconnect (JNIEnv *env, jclass c, jlong ptr, jstring endpoint) {
    const char *ep = (const char *) (*env)->GetStringUTFChars (env, endpoint, NULL);
    int rc = zsock_disconnect ((zsock_t *) ptr, "%s", ep);
    (*env)->ReleaseStringUTFChars (env, endpoint, ep);
    return rc;
}
