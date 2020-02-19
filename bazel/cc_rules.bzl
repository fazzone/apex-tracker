def config_settings():
    native.config_setting(
	      name = "windows",
	      constraint_values = ["@platforms//os:windows"],
	      visibility = ["//visibility:public"],
    )

def my_cc_library(copts = [], **kwargs):
    native.cc_library(
        copts = copts + select({
            ":windows": ["/std:c++17"],
            "//conditions:default": ["--std=c++17"],
        }),
        **kwargs
    )
    
def my_cc_binary(copts = [], **kwargs):
    native.cc_binary(
        copts = copts + select({
            ":windows": ["/std:c++17"],
            "//conditions:default": ["--std=c++17"],
        }),
        **kwargs
    )
    

        
