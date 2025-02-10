# curl gRPC

A proof of concept for sending gRPC request through a simple curl.

## Requirements

This project mainly utilizes protobuf and curl library.
Meson is also needed as it's the primary build system of this project.

- [protobuf-c](https://github.com/protobuf-c/protobuf-c)
- [curl](https://curl.se/)
- [meson](https://mesonbuild.com/)

You might need to download additional dependencies (e.g. `libcurl4-openssl-dev`) depending on your machines.

## Quickstart

``` sh
# setup the build directory
$ meson setup builddir 

# compile the project to builddir
$ meson compile -C builddir

# execute the program
$ ./builddir/curl_grpc world
```

**Mat 2025**
