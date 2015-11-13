#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "czmq.h"
#include "../../native/include/org_zeromq_czmq_ZCtx.h"

JNIEXPORT jlong JNICALL
Java_org_zeromq_czmq_ZCtx__1_1init (JNIEnv *env, jclass c) {
    zctx_t *ctx = zctx_new ();
    if(ctx) {
        return (jlong) ctx;
    }
    return -1;
}

JNIEXPORT void JNICALL
Java_org_zeromq_czmq_ZCtx__1_1destroy (JNIEnv *env, jclass c, jlong ptr) {
    zctx_t *ctx = (zctx_t *) ptr;
    zctx_destroy(&ctx);
}

JNIEXPORT void JNICALL
Java_org_zeromq_czmq_ZCtx__1_1set_1iothreads (JNIEnv *env, jclass c, jlong ptr, jint iothreads) {
    zctx_set_iothreads((zctx_t *) ptr, iothreads);
}

JNIEXPORT void JNICALL
Java_org_zeromq_czmq_ZCtx__1_1set_1linger (JNIEnv *env, jclass c, jlong ptr, jint linger) {
    zctx_set_linger((zctx_t *) ptr, linger);
}
