load(":docker_rules.bzl", "docker_image", "docker_load")
load("@rules_pkg//:pkg.bzl", "pkg_tar", "pkg_zip")

pkg_tar(
    name = "app_files",
    srcs = ["@//src:hello"],
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
