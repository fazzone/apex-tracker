load("//src/tools:surf_extract.bzl", "surf_extract")
surf_extract(
    name = "s4launch_small",
    image = "//res:mp_rr_desertlands_mu1_s4launch_small.png"
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

