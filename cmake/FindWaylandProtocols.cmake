# WaylandProtocols.cmake
# Handles Wayland protocol code generation using WaylandScanner and ECM

find_package(ECM REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_FIND_MODULE_DIR})

find_program(WAYLAND_SCANNER NAMES wayland-scanner)

execute_process(
    COMMAND pkg-config wayland-protocols --variable=pkgdatadir
    OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(WaylandScanner REQUIRED)

ecm_add_wayland_client_protocol(WL_PROT_SRC
    PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml
    BASENAME xdg-shell
    PRIVATE_CODE)
ecm_add_wayland_client_protocol(WL_DEC_PROT_SRC
    PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml
    BASENAME xdg-decoration-unstable-v1
    PRIVATE_CODE)

add_library(wayland_protocols
    STATIC
        ${WL_PROT_SRC}
        ${WL_DEC_PROT_SRC}
)

target_include_directories(wayland_protocols
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

set(WAYLAND_PROTOCOLS_LIBRARIES wayland_protocols)
set(WAYLAND_PROTOCOLS_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})
set(WAYLAND_PROTOCOLS_FOUND TRUE)