cc_library(
    name = "lib",
    srcs = ["dlib/all/source.cpp"],
    hdrs = glob(["dlib/**/*.h"]),
    includes = ["."],
    defines = [
        "DLIB_PNG_SUPPORT",
        "ENABLE_ASSERTS",
    ],
    copts = ["/arch:AVX"],
    deps = ["@libpng//:lib"],
    visibility = ["//visibility:public"], 
    linkstatic = 1,
)
