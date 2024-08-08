const std = @import("std");
pub const czmq = @cImport(@cInclude("czmq.h"));
const testing = std.testing;
const debug = std.debug;
const tls = std.crypto.tls;
const x25519 = std.crypto.dh.X25519;
const cstr = [*:0]const u8;
const str = []const u8;

test "Reference all decls" {
    _ = czmq;
    testing.refAllDecls(@This());
}

test "Hello 0MQ" {
    const msg: cstr = "Hello, Curves!\n";
    var publisher: ?*czmq.zsock_t = czmq.zsock_new(czmq.ZMQ_PUB);
    czmq.zsock_set_curve_server(publisher, @boolToInt(true));
    debug.print("\n{s}", .{msg});
    czmq.zsock_destroy(&publisher);
}
