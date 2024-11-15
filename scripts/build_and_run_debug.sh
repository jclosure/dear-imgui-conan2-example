#!/bin/bash

conan install . -s build_type=Debug --build=missing
cmake --preset conan-debug
cmake --build --preset conan-debug
cd build/Debug
./dear-imgui-conan