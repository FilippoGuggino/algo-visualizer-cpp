#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=/home/fguggino/workspace/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DPLATFORM=Web -B build.web -GNinja
cmake --build build.web/
