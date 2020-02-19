config_setting(
    name = "windows",
    constraint_values = [
        "@platforms//os:windows",
    ],
)


cc_library(
    name = "lib",
    srcs = ["sqlite3.c"],
    hdrs = glob(["sqlite3.h"]),
    defines = [
        "SQLITE_ENABLE_RTREE",
        "SQLITE_ENABLE_JSON1",
    ],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-lpthread", "-ldl"],
    }),
    visibility = ["//visibility:public"], 
    linkstatic = True,
)


cc_binary(
    name = "shell",
    srcs = ["shell.c"],
    deps = [":lib"],
    defines = [
        "SQLITE_ENABLE_RTREE",
        "SQLITE_ENABLE_JSON1",
    ],
    linkopts = select({
        ":windows": [],
        "//conditions:default": ["-lpthread", "-ldl"],
    }),
    visibility = ["//visibility:public"], 
    linkstatic = True,
)

