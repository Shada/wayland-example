#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <sys/mman.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

#include "utils/logger.hpp"
#include "wayland_client.hpp"
#include "wayland_window.hpp"

namespace tobi_engine
{


    void keyboard_map(void *data, struct wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size) 
    {
        LOG_DEBUG("keyboard_map()");

        if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
            throw std::runtime_error("Error: Unupported keymab format, currently only XKB is supported");

        auto client =static_cast<WaylandClient*>(data);

        auto keymap_shm = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));

        if(!keymap_shm)
            throw std::runtime_error("Error: Failed to load Keymap!");

        auto keymap = static_cast<xkb_keymap*>(xkb_keymap_new_from_string(client->get_xkb_context(), keymap_shm, XKB_KEYMAP_FORMAT_TEXT_V1,
            XKB_KEYMAP_COMPILE_NO_FLAGS));
        munmap(keymap_shm, size);
        close(fd);

        auto state = xkb_state_new(keymap);

        client->set_keymap(keymap);
        client->set_kb_state(state);
    }

    void keyboard_enter(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array* keys) 
    {
        LOG_DEBUG("keyboard_enter()");
        auto& window_registry = WindowRegistry::get_instance();
        auto window = static_cast<WaylandWindow*>(wl_surface_get_user_data(surface));
        
        window_registry.set_active_window(window->get_uid());

    }

    void keyboard_leave(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface) 
    {
        LOG_DEBUG("keyboard_leave()");
        auto& window_registry = WindowRegistry::get_instance();
        window_registry.unset_active_window();
    }

    void keyboard_key(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) 
    {
        auto& window_registry = WindowRegistry::get_instance();
        LOG_DEBUG("keyboard_key() = {}", key);
        uint32_t keycode = key + 8;

        window_registry.on_key(keycode, state);
    }

    void keyboard_modifiers(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group) 
    {
        LOG_DEBUG("keyboard_modifiers()");
        auto client =static_cast<WaylandClient*>(data);


       xkb_state_update_mask(client->get_state(),
               depressed, latched, locked, 0, 0, group);
    }

    void keyboard_repeat(void *data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) 
    {
        LOG_DEBUG("keyboard_repeat()");
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

    void pointer_enter(void *data, wl_pointer* pointer, uint32_t serial, wl_surface *surface, wl_fixed_t x, wl_fixed_t y)
    {
        LOG_DEBUG("pointer_enter()");
        if (!surface)
            return;

        auto& window_registry = WindowRegistry::get_instance();
        auto window =static_cast<WaylandWindow*>(wl_surface_get_user_data(surface));
        if (!window)
            return;
        window_registry.set_pointer_active_window(window->get_uid());
        
        window->pointer_event.pointer = pointer;
        window->pointer_event.serial = serial;
        window->pointer_event.position.x = wl_fixed_to_int(x);
        window->pointer_event.position.y = wl_fixed_to_int(y);
        window->pointer_event.button = 0; // No button pressed
        window->pointer_event.state = 0; // No button pressed

        window->update_cursor("nw-resize"); // Set default cursor for pointer enter

        window->on_pointer_motion(wl_fixed_to_int(x), wl_fixed_to_int(y));

    }

    void pointer_leave(void *data, wl_pointer* poiner, uint32_t serial, wl_surface *surface)
    {
        LOG_DEBUG("pointer_leave()");
        auto& window_registry = WindowRegistry::get_instance();

        window_registry.unset_pointer_active_window();

        if (!surface)
            return;
        auto window = static_cast<WaylandWindow*>(wl_surface_get_user_data(surface));
        if (!window)
            return;
        window->pointer_event.pointer = nullptr;
        window->pointer_event.serial = serial;
        window->pointer_event.position.x = 0;
        window->pointer_event.position.y = 0;
        window->pointer_event.button = 0; // No button pressed
        window->pointer_event.state = 0; // No button pressed

        window->update_cursor("none"); // Set default cursor for pointer enter

    }

    void pointer_motion(void *data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
    {
        auto& window_registry = WindowRegistry::get_instance();

        window_registry.on_pointer_motion(wl_fixed_to_int(x), wl_fixed_to_int(y));
    }

    void pointer_button(void *data, wl_pointer* poiner, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
    {
        LOG_DEBUG("pointer_button()");
        auto& window_registry = WindowRegistry::get_instance();
        window_registry.on_pointer_button(button, state);
    }

    void pointer_axis(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
    {
        LOG_DEBUG("pointer_axis()");
    }

    static const struct wl_pointer_listener pointer_listener = 
    {
        .enter = pointer_enter,
        .leave = pointer_leave,
        .motion = pointer_motion,
        .button = pointer_button,
        .axis = pointer_axis,
    };
    
}