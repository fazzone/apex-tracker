config_setting(
    name = "windows",
    constraint_values = [
        "@platforms//os:windows",
    ],
)

cc_library(
    name = "lib",
    srcs = ["dlib/all/source.cpp"],
    hdrs = glob(["dlib/**/*.h", "dlib/**/*.cpp"]),
    includes = ["."],
    defines = [
        "DLIB_PNG_SUPPORT",
        "ENABLE_ASSERTS",
    ],
    copts = select({
        ":windows": ["/arch:AVX"],
        "//conditions:default": ["-march=native"]
    }),
    deps = ["@libpng//:lib"],
    visibility = ["//visibility:public"], 
    linkstatic = 1,
)
