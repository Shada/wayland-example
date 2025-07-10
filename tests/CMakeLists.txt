cmake_minimum_required(VERSION 3.16)
project(WaylandClientTests LANGUAGES CXX)

# Fetch Catch2 using FetchContent (v3.0.1 is used here; adjust version as needed)
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)
FetchContent_MakeAvailable(catch2)

# Create unit test executable (ensure test_main.cpp exists with your tests)
add_executable(unit_tests)

target_sources(unit_tests
    PRIVATE
        wayland_types_test.cpp
        test_main.cpp
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src/events/include
        ${CMAKE_SOURCE_DIR}/src/utils/include
        ${CMAKE_CURRENT_BINARY_DIR}/../src
)

target_link_libraries(unit_tests 
    PRIVATE 
        Catch2::Catch2WithMain
        wayland_window)

# Enable testing and discover Catch2 tests automatically
include(CTest)
include(Catch)
catch_discover_tests(unit_tests)