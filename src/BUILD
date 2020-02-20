load("//bazel:cc_rules.bzl", "config_settings", "my_cc_library", "my_cc_binary")

config_settings()

my_cc_library(
    name = "surf_match",
    hdrs = ["surf_match.h"],
    srcs = ["surf_match.cpp"],
    deps = ["@dlib//:lib"],
)

my_cc_binary(
    name = "surf_test",
    srcs = ["surf_test.cpp"],
    deps = [":surf_match", "@dlib//:lib"],
)


my_cc_library(
    name = "map_matcher",
    srcs = ["map_matcher.cpp"],
    hdrs = ["map_matcher.h"],
    deps = [
        ":surf_match",
        "@dlib//:lib",
        "@sqlite//:lib"
    ],
)

my_cc_binary(
    name = "ffmpeg_pipe_png_test",
    srcs = ["ffmpeg_pipe_png_test.cpp"],
    includes = ["schema.h"],
    deps = [
        ":map_matcher",
        "@dlib//:lib"
    ],
)

my_cc_library(
    name = "read_raw_video",
    srcs = ["read_raw_video.cpp"],
    hdrs = ["schema.h"],
    defines = select({
        ":windows": ["WINDOWS_STDIN_REOPEN"],
        "//conditions:default": [],
    }),
    linkstatic = 1,
    deps = [
        ":map_matcher",
        "@dlib//:lib",
        "@sqlite//:lib"
    ],
)

my_cc_binary(
    name = "video_reader",
    deps = [":read_raw_video"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

my_cc_library(
    name = "d2d",
    srcs = ["d2d.cpp"],
    hdrs = [
        "d2d.h",
        "messages.h",
        "screenshot.h",
    ],
    deps = [
        "@dlib//:lib",
        "@sqlite//:lib",
    ],
    linkopts = [
        "user32.lib",
        "shell32.lib",
        "d2d1.lib",
        "ole32.lib",
        "oleaut32.lib",
        "windowscodecs.lib",
    ]
)

my_cc_binary(
    name = "d2d_test",
    srcs = [
        "d2dmain.cpp",
        "screenshot.cpp",
    ],
    includes = ["schema.h"],
    linkopts = [
        "/subsystem:windows",
        "gdi32.lib",
    ],
    deps = [
        ":d2d",
        ":map_matcher",
        "@dlib//:lib",
    ],
    visibility = ["//visibility:public"],
)

my_cc_binary(
    name = "hello",
    srcs = ["hello.cpp"],
    visibility = ["//visibility:public"],
)
