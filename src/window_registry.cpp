#include "../include/window_registry.hpp"
#include "wayland_window.hpp"
#include <cstddef>
#include <memory>

namespace tobi_engine
{
    std::shared_ptr<WindowRegistry> WindowRegistry::instance = nullptr;

    std::shared_ptr<WindowRegistry> WindowRegistry::get_instance()
    {
        if(!instance)
            instance = std::shared_ptr<WindowRegistry>(new WindowRegistry());

        return instance;
    }

    std::shared_ptr<Window> WindowRegistry::create_window(WindowProperties properties)
    {
        auto window = std::make_shared<WaylandWindow>(properties);
        windows[window->get_surface()] = window;
        
        return window;
    }

    void WindowRegistry::on_keypress(uint32_t key)
    {
        if(active_window)
        { 
            active_window->on_keypress(key); 
        }
    }

}