//! Works only Zig version: 0.11.0 or higher

const std = @import("std");

const pkgBuilder = struct {
    mode: std.builtin.OptimizeMode,
    target: std.zig.CrossTarget,
    build: *std.Build.CompileStep,
};

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

    // Options
    const pkg_dep = b.option(bool, "fetch", "Download libzmq with zig-pkg [default: true]") orelse true;
    const shared = b.option(bool, "shared", "Build shared library [default: true]") orelse true;

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

    const libczmq = if (shared) b.addSharedLibrary(.{
        .name = "czmq_zig",
        // In this case the main source file is merely a path, however, in more
        // complicated build scripts, this could be a generated file.
        //.root_source_file = .{ .path = "src/czmq.zig" },
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
    libczmq.addConfigHeader(config_header);
    libczmq.addIncludePath("../../include");
    libczmq.addIncludePath(config_header.include_path);
    libczmq.addCSourceFiles(lib_src, lib_flags);
    if (target.isWindows() and shared) {
        libczmq.linkSystemLibraryName("ws2_32");
        libczmq.linkSystemLibraryName("rpcrt4");
        libczmq.linkSystemLibraryName("iphlpapi");
    }
    if (pkg_dep) {
        const libzmq_dep = b.dependency("libzmq", .{
            .optimize = optimize,
            .target = target,
        });
        const libzmq = libzmq_dep.artifact("zmq");
        libczmq.linkLibrary(libzmq);
        libczmq.installLibraryHeaders(libzmq);
    } else libczmq.linkSystemLibrary("zmq");
    if (target.isLinux() and shared) {
        libczmq.linkSystemLibrary("dl");
        libczmq.linkSystemLibrary("rt");
    }

    // TODO: No support MSVC libC++ (ucrt/msvcrt/vcruntime)
    // https://github.com/ziglang/zig/issues/4785 - drop replacement for MSVC
    if (target.getAbi() != .msvc) {
        libczmq.linkLibCpp();
        libczmq.linkLibC();
    }
    // This declares intent for the library to be installed into the standard
    // location when the user invokes the "install" step (the default step when
    // running `zig build`).
    libczmq.install();
    b.installDirectory(.{
        .source_dir = "../../include",
        .install_dir = .header,
        .install_subdir = "",
        .exclude_extensions = &.{"am"},
    });

    build_test(b, .{ .target = target, .mode = optimize, .build = libczmq });
    buildSample(b, .{ .target = target, .mode = optimize, .build = libczmq }, "hello_czmq", "../../examples/security/hello.c");
}

fn buildSample(b: *std.Build.Builder, lib: pkgBuilder, name: []const u8, file: []const u8) void {
    const test_exe = b.addExecutable(.{
        .name = name,
        .optimize = lib.mode,
        .target = lib.target,
    });
    test_exe.linkLibrary(lib.build);
    test_exe.addSystemIncludePath("../../include");
    test_exe.addCSourceFile(file, lib_flags);
    if (lib.target.isWindows())
        test_exe.linkSystemLibraryName("ws2_32");
    test_exe.linkLibC();
    test_exe.install();

    const run_cmd = test_exe.run();
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", b.fmt("Run the {s}", .{name}));
    run_step.dependOn(&run_cmd.step);
}

fn build_test(b: *std.Build.Builder, pkg: pkgBuilder) void {
    // Creates a step for unit testing.
    const libtest = b.addTest(.{
        .root_source_file = .{ .path = "src/czmq.zig" },
        .target = pkg.target,
        .optimize = pkg.mode,
    });
    //    libtest.addIncludePath("../../include");
    if (pkg.target.isWindows()) {
        libtest.linkSystemLibraryName("ws2_32");
        libtest.linkSystemLibraryName("rpcrt4");
        libtest.linkSystemLibraryName("iphlpapi");
    }
    libtest.linkLibrary(pkg.build);
    if (pkg.target.getAbi() != .msvc) {
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
    "../../src/zargs.c",
    "../../src/zarmour.c",
    "../../src/zauth.c",
    "../../src/zbeacon.c",
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
    "../../src/zgossip.c",
    "../../src/zgossip_msg.c",
    "../../src/zhash.c",
    "../../src/zhashx.c",
    "../../src/zhttp_client.c",
    "../../src/zhttp_request.c",
    "../../src/zhttp_response.c",
    "../../src/zhttp_server.c",
    "../../src/zhttp_server_options.c",
    "../../src/ziflist.c",
    "../../src/zlist.c",
    "../../src/zlistx.c",
    "../../src/zloop.c",
    "../../src/zmonitor.c",
    "../../src/zmsg.c",
    "../../src/zosc.c",
    "../../src/zpoller.c",
    "../../src/zproc.c",
    "../../src/zproxy.c",
    "../../src/zrex.c",
    "../../src/zsock.c",
    "../../src/zstr.c",
    "../../src/zsys.c",
    "../../src/ztimerset.c",
    "../../src/ztrie.c",
    "../../src/zuuid.c",
};
