load(":docker_rules.bzl", "docker_image", "docker_load", "dockerfile_build")
load("@rules_pkg//:pkg.bzl", "pkg_tar", "pkg_zip")

pkg_tar(
    name = "app_files",
    srcs = [
        "//src:video_reader",
        "//res:mp_rr_desertlands_mu1_s4launch_small.png",
        "//:s4launch_small",
    ],
)

dockerfile_build(
    name = "base",
    dockerfile = "base.Dockerfile",
)

docker_image(
    name = "app",
    base = ":base",
    add_archive = ":app_files",
    prefix = "/app",
)
