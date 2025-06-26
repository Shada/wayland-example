
# Create wayland_window library
add_library(wayland_window STATIC
    wayland_window.cpp
    window_registry.cpp
    wayland_client.cpp
    shared_memory.cpp
    wayland_deleters.cpp
    window.cpp
    utils/logger.cpp
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
pkg_check_modules(wayland-client REQUIRED wayland-client)
pkg_check_modules(wayland-cursor REQUIRED wayland-cursor)
target_link_libraries(wayland_window ${wayland-client_LIBRARIES})
target_link_libraries(wayland_window ${wayland-cursor_LIBRARIES})

find_package(ECM REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_FIND_MODULE_DIR})
# Use Wayland scanner to generate protocol files (header and source) from the Wayland XML specification
find_program(WAYLAND_SCANNER NAMES wayland-scanner)

execute_process(COMMAND pkg-config wayland-protocols --variable=pkgdatadir
    OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

find_package(WaylandScanner REQUIRED)
ecm_add_wayland_client_protocol(WL_PROT_SRC
    PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml
    BASENAME xdg-shell)
ecm_add_wayland_client_protocol(WL_DEC_PROT_SRC
    PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml
    BASENAME xdg-decoration-unstable-v1)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_sources(wayland_window
    PRIVATE
        ${WL_PROT_SRC}
        ${WL_DEC_PROT_SRC}
)
