#include "wayland_display.hpp"
#include "../wayland_types.hpp"
#include <wayland-client-core.h>
#include <wayland-client.h>

namespace tobi_engine {
// Implementation is in the header for this simple class
    WaylandDisplay::WaylandDisplay() 
    {
        display = WlDisplayPtr(wl_display_connect(nullptr));
        if (!display)
            throw std::runtime_error("Failed to create Wayland Display!");
    }
}