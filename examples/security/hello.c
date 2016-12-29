//
//  CZMQ APIv3
//

#include <czmq.h>

int main (void) {
    zsock_t *publisher = zsock_new (ZMQ_PUB);
    zsock_set_curve_server (publisher, true);
    puts ("Hello, Curve!");
    zsock_destroy(&publisher);
    return 0;
}
