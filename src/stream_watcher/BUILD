load("@stream_watcher_deps//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "run_youtube_dl",
    srcs = ["run_youtube_dl.py"],
    deps = [
        requirement("youtube-dl"),
    ]
)
