# Wayland Window Example
This repository provides a minimal Wayland client and window example implemented in C++ using a CMake-based build system.

## Dependencies
CMake 3.16 or higher, a C++ compiler with C++23 support, and Wayland development libraries (for example libwayland-dev and libwayland-protocols-dev) are required. Tests also require Git to fetch Catch2 via CMake.

## Build and Run

To build the project, ensure you have the necessary dependencies installed.
In the project root, run the following commands:

```bash
cmake -B build -S .
cmake --build build
```
To run the example application, execute the following command:

```bash
./build/bin/release/app
```

You can also build and run the application in debug mode by executing:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/bin/debug/app
```

## Testing

To run the tests, execute the following commands:

```bash
ctest --test-dir build/tests
```