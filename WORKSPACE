load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

## build rules
http_archive(
    name = "rules_pkg",
    url = "https://github.com/bazelbuild/rules_pkg/releases/download/0.2.4/rules_pkg-0.2.4.tar.gz",
    sha256 = "4ba8f4ab0ff85f2484287ab06c0d871dcb31cc54d439457d28fd4ae14b18450a",
)

git_repository(
    name = "rules_python",
    remote = "https://github.com/bazelbuild/rules_python.git",
    commit = "38f86fb55b698c51e8510c807489c9f4e047480e",
)

# python requirements
load("@rules_python//python:repositories.bzl", "py_repositories")
py_repositories()

load("@rules_python//python:pip.bzl", "pip_repositories", "pip3_import")
pip_repositories()

pip3_import(
    name = "stream_watcher_deps",
    requirements = "@//src/stream_watcher:requirements.txt",
)

load("@stream_watcher_deps//:requirements.bzl", "pip_install")
pip_install()

## cc libraries
http_archive(
    name = "dlib",
    urls = ["https://github.com/davisking/dlib/archive/v19.19.tar.gz"],
    sha256 = "7af455bb422d3ae5ef369c51ee64e98fa68c39435b0fa23be2e5d593a3d45b87",
    strip_prefix = "dlib-19.19",
    build_file = "@//third_party:dlib.BUILD",
)

http_archive(
    name = "zlib",
    urls = ["https://github.com/madler/zlib/archive/v1.2.11.tar.gz"],
    sha256 = "629380c90a77b964d896ed37163f5c3a34f6e6d897311f1df2a7016355c45eff",
    strip_prefix = "zlib-1.2.11",
    build_file = "@//third_party:zlib.BUILD",
)

http_archive(
    name = "libpng",
    urls = ["https://github.com/glennrp/libpng/archive/v1.4.22rc01.tar.gz"],
    sha256 = "d7203c7981ac8833ba5c08688e7ee85d19b10931635e28d33338719bbedb25d6",
    strip_prefix = "libpng-1.4.22rc01",
    build_file = "@//third_party:libpng.BUILD",
)


http_archive(
    name = "sqlite",
    urls = ["https://www.sqlite.org/2020/sqlite-amalgamation-3310100.zip"],
    strip_prefix = "sqlite-amalgamation-3310100",
    build_file = "@//third_party:sqlite.BUILD",
)


