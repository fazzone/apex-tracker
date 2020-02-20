load("//src/tools:surf_extract.bzl", "surf_extract")
surf_extract(
    name = "s4launch_small",
    image = "//res:mp_rr_desertlands_mu1_s4launch_small.png",
    visibility = ["//visibility:public"],
)

load("@rules_pkg//:pkg.bzl", "pkg_zip")
pkg_zip(
    name = "dist",
    srcs = [
        "//res:mp_rr_desertlands_mu1_s4launch_small.png",
        ":s4launch_small",
        "//src:d2d_test",
    ],
)


platform(
    name = "x64_windows-clang-cl",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
        "@bazel_tools//tools/cpp:clang-cl",
    ],
)

platform(
    name = "x64_linux",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
)
