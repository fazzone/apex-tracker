load(":docker_rules.bzl", "docker_image", "docker_load")
load("@rules_pkg//:pkg.bzl", "pkg_tar", "pkg_zip")

pkg_tar(
    name = "app_files",
    srcs = [
        "//src:video_reader_musl",
        "//res:mp_rr_desertlands_mu1_s4launch_small.png",
        "//:s4launch_small",
    ],
)

docker_load(
    name = "base",
    archive = "@stream_watcher_base_image//file",
)

docker_image(
    name = "app",
    base = ":base",
    add_archive = ":app_files",
    prefix = "/app",
)
