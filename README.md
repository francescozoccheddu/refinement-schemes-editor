# Refinement Schemes Editor

### Compatibility

Requires [CMake](https://cmake.org/) 3.8+, a modern C++20 compiler and OpenGL 1.1 support.
Tested on MSVC v143 on Windows 11 and g++ 12.2 on Ubuntu 22.10.

## Setup

1. Clone this repository and all its submodules:

```Shell
git clone --recurse-submodules https://github.com/francescozoccheddu/refinement-schemes-editor
cd refinement-schemes-editor
```

2. Generate the build system:

```Shell
mkdir out
cd out
cmake ..
```

3. Build:

```Shell
cmake --build .
```

4. Run the `main` executable.

5. Enjoy! 😉