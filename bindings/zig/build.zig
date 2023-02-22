//! Works only Zig version: 0.11.0 or higher

const std = @import("std");

// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    // Standard target options allows the person running `zig build` to choose
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native. Other options
    // for restricting supported target set are available.
    const target = b.standardTargetOptions(.{});

    // Standard optimization options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall. Here we do not
    // set a preferred release mode, allowing the user to decide how to optimize.
    const optimize = b.standardOptimizeOption(.{});

    const pkg_dep = b.option(bool, "fetch", "Download libzmq with zig-pkg [default: false]") orelse false;

    const libzmq_dep = b.dependency("libzmq", .{
        .optimize = optimize,
        .target = target,
    });
    const libzmq = libzmq_dep.artifact("zmq");

    const config_header = if (!target.isWindows()) b.addConfigHeader(.{
        .style = .blank,
        .include_path = "platform.h",
    }, .{
        .CZMQ_BUILD_DRAFT_API = {},
        .HAVE_LINUX_WIRELESS_H = {},
        .HAVE_NET_IF_H = {},
        .HAVE_NET_IF_MEDIA_H = null,
        .HAVE_GETIFADDRS = {},
        .HAVE_FREEIFADDRS = {},
        .HAVE_DECL_AI_V4MAPPED = 0,
    }) else b.addConfigHeader(.{
        .style = .blank,
        .include_path = "platform.h",
    }, .{});

    const shared = b.option(bool, "shared", "Build shared Library [default: false]") orelse false;
    const lib = if (shared) b.addSharedLibrary(.{
        .name = "czmq_zig",
        // In this case the main source file is merely a path, however, in more
        // complicated build scripts, this could be a generated file.
        //.root_source_file = .{ .path = "src/main.zig" },
        .version = .{
            .major = 4,
            .minor = 2,
            .patch = 2,
        },
        .target = target,
        .optimize = optimize,
    }) else b.addStaticLibrary(.{
        .name = "czmq_zig",
        .root_source_file = .{ .path = "src/czmq.zig" },
        .target = target,
        .optimize = optimize,
    });
    lib.addConfigHeader(config_header);
    lib.addIncludePath("../../include");
    lib.addIncludePath(config_header.include_path);
    lib.addCSourceFiles(lib_src, lib_flags);
    if (target.isWindows() and shared) {
        lib.linkSystemLibraryName("ws2_32");
        lib.linkSystemLibraryName("rpcrt4");
        lib.linkSystemLibraryName("iphlpapi");
    }
    if (pkg_dep)
        lib.linkLibrary(libzmq)
    else
        lib.linkSystemLibrary("zmq");
    if (target.isLinux() and shared) {
        lib.linkSystemLibrary("dl");
        lib.linkSystemLibrary("rt");
    }
    if (target.getAbi() != .msvc) {
        lib.linkLibCpp();
        lib.linkLibC();
    }
    // This declares intent for the library to be installed into the standard
    // location when the user invokes the "install" step (the default step when
    // running `zig build`).
    lib.install();
    lib.installHeadersDirectory("../../include", "");
    lib.installLibraryHeaders(libzmq);

    // Creates a step for unit testing.
    const libtest = b.addTest(.{
        .root_source_file = .{ .path = "src/czmq.zig" },
        .target = target,
        .optimize = optimize,
    });
    libtest.addConfigHeader(config_header);
    libtest.addIncludePath(config_header.include_path);
    libtest.addIncludePath("../../include");
    libtest.addCSourceFiles(lib_src, lib_flags);
    if (target.isWindows() and shared) {
        libtest.linkSystemLibraryName("ws2_32");
        libtest.linkSystemLibraryName("rpcrt4");
        libtest.linkSystemLibraryName("iphlpapi");
    }
    if (pkg_dep)
        libtest.linkLibrary(libzmq)
    else
        libtest.linkSystemLibrary("zmq");
    if (target.getAbi() != .msvc) {
        libtest.linkLibCpp();
        libtest.linkLibC();
    }
    // This creates a build step. It will be visible in the `zig build --help` menu,
    // and can be selected like this: `zig build test`
    // This will evaluate the `test` step rather than the default, which is "install".
    const test_step = b.step("test", "Run library tests");
    test_step.dependOn(&libtest.step);
}

const lib_flags: []const []const u8 = &.{
    "-std=gnu99",
    "-O3",
    "-Wall",
    "-pedantic",
};
const lib_src: []const []const u8 = &.{
    "../../src/zactor.c",
    "../../src/zarmour.c",
    "../../src/zcert.c",
    "../../src/zcertstore.c",
    "../../src/zchunk.c",
    "../../src/zclock.c",
    "../../src/zconfig.c",
    "../../src/zdigest.c",
    "../../src/zdir.c",
    "../../src/zdir_patch.c",
    "../../src/zfile.c",
    "../../src/zframe.c",
    "../../src/zhash.c",
    "../../src/zhashx.c",
    "../../src/ziflist.c",
    "../../src/zlist.c",
    "../../src/zlistx.c",
    "../../src/zloop.c",
    "../../src/zmsg.c",
    "../../src/zpoller.c",
    "../../src/zsock.c",
    "../../src/zstr.c",
    "../../src/zsys.c",
    "../../src/zuuid.c",
    "../../src/zauth.c",
    "../../src/zbeacon.c",
    "../../src/zgossip.c",
    "../../src/zmonitor.c",
    "../../src/zproxy.c",
    "../../src/zrex.c",
    "../../src/zgossip_msg.c",
    "../../src/ztrie.c",
    "../../src/zargs.c",
    "../../src/zproc.c",
    "../../src/ztimerset.c",
    "../../src/zhttp_server.c",
    "../../src/zhttp_client.c",
    "../../src/zhttp_request.c",
    "../../src/zhttp_response.c",
    "../../src/zhttp_server_options.c",
    "../../src/zosc.c",
};
