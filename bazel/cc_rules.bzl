
def config_settings():
    native.config_setting(
	      name = "windows_msvc",
	      constraint_values = [
		        "@platforms//os:windows",
		        "@bazel_tools//tools/cpp:msvc",
	      ],
	      visibility = ["//visibility:public"],
    )
    native.config_setting(
	      name = "windows_clang-cl",
	      constraint_values = [
		        "@platforms//os:windows",
		        "@bazel_tools//tools/cpp:clang-cl",
	      ],
	      visibility = ["//visibility:public"],
    )

def my_cc_library(copts = [], **kwargs):
    native.cc_library(
        copts = copts + select({
            ":windows_msvc": ["/std:c++17"], 
            ":windows_clang-cl": ["/std:c++17"],
            "//conditions:default": ["--std=c++17"],
        }),
        **kwargs
    )
    
def my_cc_binary(copts = [], **kwargs):
    native.cc_binary(
        copts = copts + select({
            ":windows_msvc": ["/std:c++17"], 
            ":windows_clang-cl": ["/std:c++17"],
            "//conditions:default": ["--std=c++17"],
        }),
        **kwargs
    )
    

        
