def _impl(ctx):
    print(ctx.attr.thing)
    print(ctx.attr.thing[PyInfo])
    # print(ctx.attr.thing.PyCcLinkParamsProvider)

    for f in ctx.files.thing:
        print("root", f.root.path, "path", f.path)
    print(ctx.attr.thing[PyInfo].imports)
    pass

nop_rule = rule(
    _impl,
    attrs = {"thing": attr.label()}
)

def docker_image(
        name = None,
        base = None,
        add_archive = None,
        prefix = None,
):

    native.genrule(
        name = name,
        message = "docker image {}".format(name),
        srcs = [add_archive],
        outs = ["image_id/" + name],
        # We can't be cached becuase our output is really the fact that we loaded the image into our local Docker
        tags = ["local", "docker-image"],
        cmd = """
        set -ex
        mkdir docker-build-root
        ARCHIVE=$(location {archive})
        ARCHIVE_NAME=$$(basename $$ARCHIVE)
        cp $(location {archive}) docker-build-root/$$ARCHIVE_NAME
        cat >docker-build-root/Dockerfile <<EOF
FROM {base}
COPY $$ARCHIVE_NAME /__$$ARCHIVE_NAME
RUN mkdir -p {prefix} && cd {prefix} && unzip /__$$ARCHIVE_NAME && rm /__$$ARCHIVE_NAME
EOF
        docker build --iidfile $(OUTS) docker-build-root
        echo "`cat $(OUTS)`"
    """.format(
        archive = add_archive,
        base = base,
        prefix = prefix,
        )
)
