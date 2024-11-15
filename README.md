# An introduction to the ImGui library

[An introduction to the ImGui library](https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html)


## Instructions

### To build and run (automatically):

Convenience build and run scripts have been created here:

- ./scripts/build_and_run_debug.sh
- ./scripts/build_and_run_release.sh

### To build and run (manually):
```
conan install . --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
cd build/Release
./dear-imgui-conan
```

## Debugging

NOTE: There is a custom `.vscode/settings.json`. See:
```json
"miDebuggerPath": "${env:HOME}/.vscode/extensions/ms-vscode.cpptools-1.22.11-darwin-arm64/debugAdapters/lldb-mi/bin/lldb-mi"
```
Update this path ^ to match the version of `cpptools` in your system.

This project has both:
- a CMake:Debug configuration.  Use the CMake controls at the bottom of the IDE to debug
- a launch configuration that can be used with Ctrl-F5 / F5 

There may be issues with opengl not showing with F5 debugging, use CMake debug controls instead


