def surf_extract(name = None, image = None, **kwargs):
    native.genrule(
        name = name,
        srcs = [image],
        tools = ["//src/tools:surf_extract"],
        outs = [name + ".surf.dat"],
        cmd = """
        $(execpath //src/tools:surf_extract) -i $< -o $@
""",
        **kwargs
    )
