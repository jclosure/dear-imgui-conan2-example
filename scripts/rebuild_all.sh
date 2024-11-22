#!/bin/bash

rm -rf ./build

# release
conan install . -s build_type=Release --build=missing
cmake --preset conan-release
cmake --build --preset conan-release

# debug
conan install . -s build_type=Debug --build=missing
cmake --preset conan-debug
cmake --build --preset conan-debug

