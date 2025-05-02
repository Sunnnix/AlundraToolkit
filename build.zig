//! Run this file with `zig build run` to build and execute the project.
//! NOTE: You might need to run with `-Doptimize=ReleaseFast` to get a working executable on some platforms.
//! You need to have zig installed https://ziglang.org/
//! This is an alternative to using a Makefile for example to build to different platforms.
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addExecutable(.{
        .name = "Alundra",
        .target = target,
        .optimize = optimize,
    });

    lib.linkLibCpp();

    const source_files = &.{
        "src/Loader/main.cpp",
        "src/Loader/engine.cpp",
        "src/Loader/datahandler.cpp",
        "src/Loader/entities.cpp",
        "src/Loader/imaging.cpp",
        "src/Loader/level.cpp",
    };

    lib.addCSourceFiles(.{
        .files = source_files,
        .flags = &.{ "-std=c++17", "-Wno-narrowing" },
    });

    lib.addIncludePath(b.path("src/Loader"));

    lib.linkSystemLibrary("SDL2");
    lib.linkSystemLibrary("GLEW");

    lib.linkFramework("OpenGL");

    b.installArtifact(lib);

    const run_cmd = b.addRunArtifact(lib);
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run the executable");
    run_step.dependOn(&run_cmd.step);
}
