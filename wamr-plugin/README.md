# Introduction

This folder contains the source code to build a plugin for [WAMR](https://github.com/bytecodealliance/wasm-micro-runtime) so as to fulfill the API of `spectest` `print_char`.

## Build

A possible command to build it is:

```bash
clang pluign.c -shared -o libmoon_run.dylib
```

## Run

Pass the file with the `--native-lib` command as:

```bash
iwasm --native-lib=libmain_test.dylib main.wasm
```