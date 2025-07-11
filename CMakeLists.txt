cmake_minimum_required(VERSION 3.21)

project(TobiWaylandWindow VERSION 1.0 DESCRIPTION "Tobi Wayland Window Example" LANGUAGES C CXX)

# C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories (platform-aware, multi-config generator support)
if(CMAKE_GENERATOR MATCHES "Visual Studio" OR CMAKE_CONFIGURATION_TYPES)
  # Multi-config generator (e.g., Visual Studio, Xcode)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib")
else()
  # Single-config generator (e.g., Makefiles, Ninja)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib")
endif()

# Default build type for single-config generators
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  message(STATUS "Defaulting build type to Release")
endif()

# Platform-specific placeholders
if(UNIX AND NOT APPLE)
  message(STATUS "Configuring for Linux")
elseif(WIN32)
  message(WARNING "Windows support is experimental")
elseif(APPLE)
  message(WARNING "Apple support is experimental")
endif()

# Options
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" OFF)

# Documentation (optional)
include(cmake/Doxygen.cmake OPTIONAL)

# Subdirectories
add_subdirectory(src)
if(BUILD_EXAMPLES)
  add_subdirectory(example)
endif()
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
