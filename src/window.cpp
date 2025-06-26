#include "window.hpp"
#include "wayland_window.hpp"
#include <memory>

namespace tobi_engine
{
    std::shared_ptr<Window> create_window(WindowProperties properties)
    {
        return std::make_shared<WaylandWindow>(properties);
    }
}