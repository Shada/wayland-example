# CMakeLists.txt for the example project

add_executable(app
    main.cpp
)

# Link the wayland_window library to the app executable
target_link_libraries(app PRIVATE wayland_window)

# Set the include directories for the wayland_window library
target_include_directories(app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

