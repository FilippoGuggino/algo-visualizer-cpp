#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=/home/peano/workspace/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DPLATFORM=Web -B build.web
cmake --build build.web/ -j12
