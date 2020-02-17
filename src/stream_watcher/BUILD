load("@stream_watcher_deps//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")
load(":docker_rules.bzl", "docker_image", "nop_rule")
load("@rules_pkg//:pkg.bzl", "pkg_tar", "pkg_zip")

py_binary(
    name = "run_youtube_dl",
    srcs = ["run_youtube_dl.py"],
    deps = [
        requirement("youtube-dl"),
    ]
)

py_binary(
    name = "test",
    srcs = ["test.py"],
    data = [
        "@//src:hello"
    ]
)


pkg_zip(
    name = "app_files",
    srcs = [":test"],
)

nop_rule(
    name = "x",
    thing = requirement("youtube-dl"), 
)

docker_image(
    name = "app",
    base = "alpine@sha256:ddba4d27a7ffc3f86dd6c2f92041af252a1f23a8e742c90e6e1297bfa1bc0c45",
    add_archive = ":app_files",
    prefix = "/app",
)
