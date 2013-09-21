#include <czmq.h>

int main (void) {
    zctx_t *ctx = zctx_new ();
    void *publisher = zsocket_new (ctx, ZMQ_PUB);
    zsocket_set_curve_server (publisher, true);
    puts ("Hello, Curve!");
    zctx_destroy (&ctx);
    return 0;
}
