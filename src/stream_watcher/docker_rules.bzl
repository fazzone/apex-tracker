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

def docker_load(name = None, archive = None, **kwargs):
    native.genrule(
        name = name,
        srcs = [archive],
        outs = ["image_id/" + name],
        tags = ["local"],
        cmd = "docker load -i $< | sed -e 's/Loaded image ID: //g' > $@",
        **kwargs,
    )

def dockerfile_build_and_save(name = None, dockerfile = None, **kwargs):
    native.genrule(
        name = name,
        srcs = [dockerfile],
        outs = [name + ".tar.gz"],
        cmd = """
docker build --iidfile {name}.iid - < $<
docker save $$(cat {name}.iid) | gzip > $@
""".format(
    name = name,
),
        **kwargs
    )

def dockerfile_build(name = None, dockerfile = None, **kwargs):
    native.genrule(
        name = name,
        srcs = [dockerfile],
        tags = ["local"],
        outs = ["image_id/" + name],
        cmd = "docker build --iidfile $@ - < $<",
        **kwargs
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
        srcs = [add_archive, base],
        outs = ["image_id/" + name],
        # We can't be cached becuase our output is really the fact that we loaded the image into our local Docker
        tags = ["local", "docker-image"],
        cmd = """
        set -ex
        mkdir docker-build-root
        ARCHIVE=$(location {archive})
        ARCHIVE_NAME=$$(basename $$ARCHIVE)
        BASE_IMAGE=$$(cat $(location {base}))
        cp $(location {archive}) docker-build-root/$$ARCHIVE_NAME
        cat >docker-build-root/Dockerfile <<EOF
FROM $$BASE_IMAGE
ADD $$ARCHIVE_NAME {prefix}
EOF
        docker build --iidfile $(OUTS) docker-build-root
        echo "`cat $(OUTS)`"
    """.format(
        archive = add_archive,
        base = base,
        prefix = prefix,
        )
)
