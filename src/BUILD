
cc_library(
    name = "surf_match",
    hdrs = ["surf_match.h"],
    srcs = ["surf_match.cpp"],
    copts = ["/std:c++17"],
    deps = ["@dlib//:lib"],
)

cc_binary(
    name = "surf_test",
    srcs = ["surf_test.cpp"],
    copts = ["/std:c++17"],
    deps = [":surf_match", "@dlib//:lib"],
)

cc_binary(
    name = "ffmpeg_pipe_png_test",
    srcs = ["ffmpeg_pipe_png_test.cpp"],
    copts = ["/std:c++17"],
    deps = [
        ":surf_match",
        "@dlib//:lib"
    ],
)

cc_library(
    name = "map_matcher",
    srcs = ["map_matcher.cpp"],
    hdrs = ["map_matcher.h"],
    copts = ["/std:c++17"],
    deps = [
        ":surf_match",
        "@dlib//:lib"
    ],
)

cc_library(
    name = "d2d",
    srcs = ["d2d.cpp"],
    hdrs = [
        "d2d.h",
        "messages.h",
        "screenshot.h",
    ],
    copts = ["/std:c++17"],
    linkopts = [
       "user32.lib",
        "d2d1.lib",
        "ole32.lib",
        "oleaut32.lib",
        "windowscodecs.lib",
    ]
)

cc_binary(
    name = "d2d_test",
    srcs = [
        "d2dmain.cpp",
        "screenshot.cpp",
    ],
    copts = ["/std:c++17"],
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


