load("//bazel:cc_rules.bzl", "config_settings", "my_cc_library", "my_cc_binary")

config_settings()

my_cc_binary(
    name = "surf_extract",
    srcs = ["surf_extract.cpp"],
    deps = ["@dlib//:lib", "@sqlite//:lib"],
    visibility = ["//visibility:public"],
)

