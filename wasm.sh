#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=$EMSDK_ROOT/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DPLATFORM=Web -B build.web -GNinja
cmake --build build.web/
