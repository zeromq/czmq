const std = @import("std");
const czmq = @cImport(@cInclude("czmq.h"));
const testing = std.testing;

test "Reference all decls" {
    _ = czmq;
    testing.refAllDecls(@This());
}

test "Hello 0MQ" {
    var publisher: ?*czmq.zsock_t = czmq.zsock_new(czmq.ZMQ_PUB);
    czmq.zsock_set_curve_server(publisher, @boolToInt(true));
    std.debug.print("\nHello, Curves!{s}", .{"\n"});
    czmq.zsock_destroy(&publisher);
}
