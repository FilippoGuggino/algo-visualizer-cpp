#!/bin/bash

cmake -DCMAKE_TOOLCHAIN_FILE=/home/xubuntu/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DPLATFORM=Web -B build.web
cmake --build build.web/ -j12
