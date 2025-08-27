#include "window.hpp"

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

    uint64_t Window::get_uid() 
    { 
        return uid;
    }

}