FROM alpine@sha256:ddba4d27a7ffc3f86dd6c2f92041af252a1f23a8e742c90e6e1297bfa1bc0c45
WORKDIR /opt
RUN wget https://johnvansickle.com/ffmpeg/builds/ffmpeg-git-amd64-static.tar.xz && tar --strip-components=1 -xJf ffmpeg-git-amd64-static.tar.xz

FROM ubuntu@sha256:d050ed7278c16ff627e4a70d7d353f1a2ec74d8a0b66e5a865356d92f5f6d87b
RUN apt-get update && apt-get install -y python3 nscd ca-certificates && ln -s /usr/bin/python3 /usr/bin/python
ADD https://yt-dl.org/downloads/latest/youtube-dl /opt/youtube-dl
RUN chmod +x /opt/youtube-dl
COPY --from=0 /opt/ffmpeg /opt
ENV PATH="/opt/:${PATH}"
