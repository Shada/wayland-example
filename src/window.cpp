#include "window.hpp"

#include <memory>

#include "wayland_window.hpp"
#include "utils/utils.hpp"

namespace tobi_engine
{

    Window::Window(const WindowProperties &properties) 
        :   properties(properties), 
            uid(generate_uid()) 
    {

    }

    std::shared_ptr<Window> create_window(const WindowProperties &properties)
    {
        return std::make_shared<WaylandWindow>(properties);
    }
}