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
    ] + select({
        ":windows": [],
        "//conditions:default": ["DLIB_NO_GUI_SUPPORT"],
    }),
    copts = select({
        ":windows": ["/arch:AVX"],
        "//conditions:default": ["-march=native"]
    }),
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-lpthread"],
    }),
    deps = ["@libpng//:lib"],
    visibility = ["//visibility:public"], 
    linkstatic = 1,
)
