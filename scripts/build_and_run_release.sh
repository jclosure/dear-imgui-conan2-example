#!/bin/bash

// conan install defaults to Release
conan install . --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
cd build/Release
./dear-imgui-conan