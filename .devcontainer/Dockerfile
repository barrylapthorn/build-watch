FROM alpine:latest

RUN apk update && \
    apk add --no-cache \
        ssh \
        build-essential \
        gcc \
        g++ \
        gdb \
        clang \
        cmake \
        rsync \
        tar \
        python \
  && apt-get clean

