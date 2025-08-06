#include "window.hpp"

#include <memory>

#include "wayland_window.hpp"
#include "utils/utils.hpp"
#include "utils/logger.hpp"

namespace tobi_engine
{

    Window::Window(const WindowProperties &properties) 
        :   properties(properties), 
            uid(generate_uid()) 
    {
        LOG_DEBUG("Window created with UID: {}", uid);
    }

}