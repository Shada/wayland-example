#include "window_registry.hpp"

#include <cstddef>
#include <memory>

#include "wayland_window.hpp"

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
        windows[window->get_uid()] = window;
        
        return window;
    }

    void WindowRegistry::on_key(uint32_t key, uint32_t state)
    {
        if(keyboard_active_window)
            keyboard_active_window->on_key(key, state); 
    }

    void WindowRegistry::on_pointer_button(uint32_t button, uint32_t state)
    {
        if(pointer_active_window)
            pointer_active_window->on_pointer_button(button, state);
    }
    void WindowRegistry::on_pointer_motion(int32_t x, int32_t y)
    {
        if(pointer_active_window)
            pointer_active_window->on_pointer_motion(x, y);
    }

}