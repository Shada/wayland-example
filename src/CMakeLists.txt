
# Create wayland_window library
add_library(wayland_window STATIC
    wayland_window.cpp
    wayland_client.cpp
    xdg-shell-protocol.c
    window.cpp
)

# Set the include directories for the wayland_window library
target_include_directories(wayland_window
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set the include directories for the wayland_window library
target_include_directories(wayland_window 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

# Find and link the Wayland client library
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)
target_link_libraries(wayland_window PkgConfig::WAYLAND_CLIENT)
