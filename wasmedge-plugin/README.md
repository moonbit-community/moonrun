# Introduction

This folder contains the source code to build a plugin for [WasmEdge](https://wasmedge.org/) so as to fulfill the API of `spectest` `print_char`.

Notice that WasmEdge changed the API at version 14.0, so there are two files in this folder.

## Build

To build them, a possible command is:

```bash
clang -std=c11 -DWASMEDGE_PLUGIN -lwasmedge -shared -o libwasmedgeMoonRun.dylib plugin_0.13.0.c
```

Depending on the platform, you should choose to generate `.dll` or `.so` or `.dylib`. Depending on your path configuration, you may need to provide `-I/path/to/include` or `-L/path/to/lib`.

## Run

To use the generated plugin, a possible command is:

```bash
WASMEDGE_PLUGIN_PATH=. wasmedge --enable-all target/wasm/release/build/main/main.wasm
```

The path depends on whether you are building with or without gc. The plugin path is for finding the dynamic library we just created. Other possible solutions are listed in the [WasmEdge documentation](https://wasmedge.org/docs/contribute/plugin/intro#loading-plug-in-from-paths)