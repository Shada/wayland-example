#include "window_registry.hpp"

#include <cstddef>
#include <memory>

#include "wayland_client.hpp"
#include "wayland_window.hpp"

namespace tobi_engine
{
    
    WindowRegistry& WindowRegistry::get_instance()
    {
        static WindowRegistry instance;
        return instance;
    }

    std::shared_ptr<Window> WindowRegistry::create_window(WindowProperties properties)
    {
        auto window = WaylandClient::get_instance().create_window(properties);
        windows[window->get_uid()] = window;
        
        return window;
    }

    WindowRegistry::WindowRegistry()
    {
        WaylandClient::get_instance();
    }

    void WindowRegistry::on_key(uint32_t key, uint32_t state)
    {
        if (!keyboard_active_window.expired())
            keyboard_active_window.lock()->on_key(key, state); 
    }

    void WindowRegistry::on_pointer_button(uint32_t button, uint32_t state)
    {
        if (!pointer_active_window.expired())
            pointer_active_window.lock()->on_pointer_button(button, state);
    }
    void WindowRegistry::on_pointer_motion(int32_t x, int32_t y)
    {
        if (!pointer_active_window.expired())
            pointer_active_window.lock()->on_pointer_motion(x, y);
    }

}