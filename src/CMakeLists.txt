
# Create wayland_window library
add_library(wayland_window STATIC
    wayland_window.cpp
    window_registry.cpp
    wayland_client.cpp
    shared_memory.cpp
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

# Use Wayland scanner to generate protocol files (header and source) from the Wayland XML specification
find_program(WAYLAND_SCANNER NAMES wayland-scanner)

# Allow configuration of the Wayland protocols path
set(WAYLAND_PROTOCOLS_PATH "/usr/share/wayland-protocols/stable" CACHE PATH "Path to Wayland protocols")
if(WAYLAND_SCANNER)
    set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
    file(MAKE_DIRECTORY ${GENERATED_DIR})
    add_custom_command(
        OUTPUT ${GENERATED_DIR}/xdg-shell-protocol.c ${GENERATED_DIR}/xdg-shell-client-protocol.h
        COMMAND ${WAYLAND_SCANNER} private-code ${WAYLAND_PROTOCOLS_PATH}/xdg-shell/xdg-shell.xml ${GENERATED_DIR}/xdg-shell-protocol.c
        COMMAND ${WAYLAND_SCANNER} client-header ${WAYLAND_PROTOCOLS_PATH}/xdg-shell/xdg-shell.xml ${GENERATED_DIR}/xdg-shell-client-protocol.h
        DEPENDS ${WAYLAND_PROTOCOLS_PATH}/xdg-shell/xdg-shell.xml
        COMMENT "Generating xdg-shell protocol files for Wayland communication"
    )
    add_custom_target(generate_xdg_shell_protocol DEPENDS ${GENERATED_DIR}/xdg-shell-protocol.c ${GENERATED_DIR}/xdg-shell-client-protocol.h)
    add_dependencies(wayland_window generate_xdg_shell_protocol)

    target_include_directories(wayland_window
        PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}/
    )
    target_sources(wayland_window
        PRIVATE
            ${GENERATED_DIR}/xdg-shell-protocol.c
    )

else()
    message(FATAL_ERROR "wayland-scanner not found. Please install the Wayland development package (e.g., 'libwayland-dev' on Debian-based systems).")
endif()

