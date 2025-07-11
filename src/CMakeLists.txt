# Create wayland_window library
add_library(wayland_window STATIC
    wayland_window.cpp
    window_registry.cpp
    wayland_input_manager.cpp
    wayland_client.cpp
    wayland_surface_buffer.cpp
    wayland_surface.cpp
    wayland_cursor.cpp
    wayland_display.cpp
    wayland_registry.cpp
    window.cpp
    utils/logger.cpp
    utils/utils.cpp
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
        ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(wayland_window PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
)

# Add cmake/ to CMAKE_MODULE_PATH for custom find modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Find and link the Wayland client library
find_package(WaylandClient REQUIRED)
find_package(WaylandCursor REQUIRED)
find_package(XKBCommon REQUIRED)
find_package(WaylandProtocols REQUIRED)

target_include_directories(wayland_window 
    PRIVATE 
        ${WAYLAND_CLIENT_INCLUDE_DIRS} 
        ${WAYLAND_CURSOR_INCLUDE_DIRS} 
        ${XKB_COMMON_INCLUDE_DIRS}
        ${WAYLAND_PROTOCOLS_INCLUDE_DIRS})
target_link_libraries(wayland_window 
    ${WAYLAND_CLIENT_LIBRARIES} 
    ${WAYLAND_CURSOR_LIBRARIES} 
    ${XKB_COMMON_LIBRARIES}
    ${WAYLAND_PROTOCOLS_LIBRARIES})

add_subdirectory(events)
target_link_libraries(wayland_window events)
