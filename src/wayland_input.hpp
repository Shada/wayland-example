#pragma once

#include <cstdint>
#include <wayland-client-protocol.h>

#include "utils/logger.hpp"

namespace tobi_engine
{

    void keyboard_map(void *data, struct wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size) 
    {
        Logger::debug("keyboard_map()");
    }

    void keyboard_enter(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array* keys) 
    {
        Logger::debug("keyboard_enter()");
        auto window_registry = WindowRegistry::get_instance();
        auto window =static_cast<Window*>(wl_surface_get_user_data(surface));
        
        window_registry->set_active_window(window->get_uid());

    }

    void keyboard_leave(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface) 
    {
        Logger::debug("keyboard_leave()");
        auto window_registry = WindowRegistry::get_instance();
        window_registry->unset_active_window();
    }

    void keyboard_key(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t t, uint32_t key, uint32_t status) 
    {
        auto window_registry = WindowRegistry::get_instance();
        Logger::debug("keyboard_key() " + std::to_string(key));
        
        window_registry->on_keypress(key);

        /*if(key == 30)
            xdg_toplevel_set_fullscreen(x_top, NULL);
        if(key == 32)
            xdg_toplevel_unset_fullscreen(x_top);*/
    }

    void keyboard_modifiers(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t group) 
    {
        Logger::debug("keyboard_modifiers()");
    }

    void keyboard_repeat(void *data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) 
    {
        Logger::debug("keyboard_repeat()");
    }

    static const struct wl_keyboard_listener keyboard_listener = 
    {
        .keymap = keyboard_map,
        .enter = keyboard_enter,
        .leave = keyboard_leave,
        .key = keyboard_key,
        .modifiers = keyboard_modifiers,
        .repeat_info = keyboard_repeat
    };
    
}