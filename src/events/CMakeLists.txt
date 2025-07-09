
add_library(events STATIC
    pointer_event.cpp
    event_registry.cpp
)

# Set the include directories for the events library
target_include_directories(events
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set the include directories for the wayland_window library
target_include_directories(wayland_window 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

