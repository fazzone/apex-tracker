load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

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

git_repository(
    name = "io_bazel_rules_docker",
    remote = "https://github.com/bazelbuild/rules_docker.git",
    commit = "363e12da417e6fa9dd447af5411b14489ea37ac4",
)

# docker
http_file(
    name = "stream_watcher_base_image",
    urls = ["https://apex-tracker.s3-us-west-2.amazonaws.com/apex-tracker-base-docker.tar.gz"],
    downloaded_file_path = "apex-tracker-base-docker.tar.gz",
)


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

### Use clang-cl on windows
# register_execution_platforms(":x64_windows-clang-cl")
# register_toolchains("@local_config_cc//:cc-toolchain-x64_windows-clang-cl")
