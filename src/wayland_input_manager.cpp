#include "wayland_input_manager.hpp"

#include "utils/logger.hpp"
#include "wayland_window.hpp"

#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <expected>

namespace tobi_engine
{

    WaylandInputManager::WaylandInputManager(const WaylandRegistry* registry)
    {   
        auto seat = registry->get_seat();
        if (!seat)
        {
            throw std::runtime_error("Failed to get Wayland seat from registry");
        }
        
        static constexpr wl_seat_listener seat_listener
        {
            &WaylandInputManager::seat_capabilities,
            &WaylandInputManager::seat_name
        };
        wl_seat_add_listener(seat, &seat_listener, this);
        LOG_DEBUG("WaylandInputManager initialized with seat");

        kb_context = XkbContextPtr(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
    }

    void WaylandInputManager::seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) 
    {
        LOG_DEBUG("seat_capabilities() = {}", capabilities);
        auto self = static_cast<WaylandInputManager*>(data);
        self->on_seat_capabilities(seat, capabilities);
    }

    void WaylandInputManager::on_seat_capabilities(struct wl_seat* seat, uint32_t capabilities)
    {
        if (capabilities & WL_SEAT_CAPABILITY_POINTER) 
        {
            if (!pointer) 
            {
                static constexpr wl_pointer_listener pointer_listener = 
                {
                    &WaylandInputManager::pointer_enter,
                    &WaylandInputManager::pointer_leave,
                    &WaylandInputManager::pointer_motion,
                    &WaylandInputManager::pointer_button,
                    &WaylandInputManager::pointer_axis,
                };

                pointer = WlPointerPtr(wl_seat_get_pointer(seat));
                wl_pointer_add_listener(pointer.get(), &pointer_listener, this);
                LOG_DEBUG("Pointer device added");
            }
        } 
        else 
        {
            pointer.reset();
            LOG_DEBUG("Pointer device removed");
        }

        if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) 
        {
            if (!keyboard) 
            {

                static constexpr wl_keyboard_listener keyboard_listener = 
                {
                    &WaylandInputManager::keyboard_map,
                    &WaylandInputManager::keyboard_enter,
                    &WaylandInputManager::keyboard_leave,
                    &WaylandInputManager::keyboard_key,
                    &WaylandInputManager::keyboard_modifiers,
                    &WaylandInputManager::keyboard_repeat
                };
                keyboard = WlKeyboardPtr(wl_seat_get_keyboard(seat));
                wl_keyboard_add_listener(keyboard.get(), &keyboard_listener, this);
                LOG_DEBUG("Keyboard device added");
            }
        } 
        else 
        {
            keyboard.reset();
            LOG_DEBUG("Keyboard device removed");
        }

        if (capabilities & WL_SEAT_CAPABILITY_TOUCH) 
        {
            // Touch support can be implemented here
            LOG_DEBUG("Touch device support is not implemented yet");
        }
    }

    void WaylandInputManager::seat_name(void* data, struct wl_seat* seat, const char* name) 
    {
        auto self = static_cast<WaylandInputManager*>(data);
        self->on_seat_name(seat, name);
    }
    void WaylandInputManager::on_seat_name(struct wl_seat* seat, const char* name)
    {
        if (name && *name) 
        {
            LOG_DEBUG("Seat name: {}", name);
        } 
        else 
        {
            LOG_DEBUG("Seat name is empty or null");
        }
    }

    void WaylandInputManager::pointer_enter(void *data, wl_pointer* pointer, uint32_t serial, wl_surface *surface, wl_fixed_t x, wl_fixed_t y)
    {
        LOG_DEBUG("pointer_enter()");
        if (!surface)
            return;

        auto window = static_cast<WaylandWindow*>(wl_surface_get_user_data(surface));
        if (!window)
            return;

        auto self = static_cast<WaylandInputManager*>(data);
        self->set_pointer_active_window(window);

        window->pointer_event.pointer = pointer;
        window->pointer_event.serial = serial;
        window->pointer_event.position.x = wl_fixed_to_int(x);
        window->pointer_event.position.y = wl_fixed_to_int(y);
        window->pointer_event.button = 0; // No button pressed
        window->pointer_event.state = 0; // No button pressed

        window->update_cursor("nw-resize"); // Set default cursor for pointer enter

        window->on_pointer_motion(wl_fixed_to_int(x), wl_fixed_to_int(y));

    }

    void WaylandInputManager::pointer_leave(void *data, wl_pointer* poiner, uint32_t serial, wl_surface *surface)
    {
        LOG_DEBUG("pointer_leave()");

        auto self = static_cast<WaylandInputManager*>(data);
        self->unset_pointer_active_window();

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

    void WaylandInputManager::pointer_motion(void *data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
    {
        LOG_DEBUG("pointer_motion()");

        auto self = static_cast<WaylandInputManager*>(data);
        self->pointer_active_window->on_pointer_motion(wl_fixed_to_int(x), wl_fixed_to_int(y));
    }

    void WaylandInputManager::pointer_button(void *data, wl_pointer* poiner, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
    {
        LOG_DEBUG("pointer_button()");
        
        auto self = static_cast<WaylandInputManager*>(data);
        self->pointer_active_window->on_pointer_button(button, state);
    }

    void WaylandInputManager::pointer_axis(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
    {
        LOG_DEBUG("pointer_axis()");
    }


    void WaylandInputManager::keyboard_map(void *data, struct wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size) 
    {
        LOG_DEBUG("keyboard_map()");
        
        auto self =static_cast<WaylandInputManager*>(data);

        if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
            throw std::runtime_error("Error: Unupported keymab format, currently only XKB is supported");

        auto keymap_shm = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));

        if(!keymap_shm)
            throw std::runtime_error("Error: Failed to load Keymap!");

        auto keymap = static_cast<xkb_keymap*>(xkb_keymap_new_from_string(self->get_xkb_context(), keymap_shm, XKB_KEYMAP_FORMAT_TEXT_V1,
            XKB_KEYMAP_COMPILE_NO_FLAGS));
        munmap(keymap_shm, size);
        close(fd);

        auto state = xkb_state_new(keymap);

        self->set_keymap(keymap);
        self->set_kb_state(state);
    }

    void WaylandInputManager::keyboard_enter(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array* keys) 
    {
        LOG_DEBUG("keyboard_enter()");

        auto window = static_cast<WaylandWindow*>(wl_surface_get_user_data(surface));
        
        auto self = static_cast<WaylandInputManager*>(data);
        self->set_keyboard_active_window(window);

    }

    void WaylandInputManager::keyboard_leave(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface) 
    {
        LOG_DEBUG("keyboard_leave()");
        
        auto self = static_cast<WaylandInputManager*>(data);
        self->unset_keyboard_active_window();
    }

    void WaylandInputManager::keyboard_key(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) 
    {
        LOG_DEBUG("keyboard_key() = {}", key);
        uint32_t keycode = key + 8;

        auto self = static_cast<WaylandInputManager*>(data);
        self->keyboard_active_window->on_key(keycode, state);
    }

    void WaylandInputManager::keyboard_modifiers(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group) 
    {
        LOG_DEBUG("keyboard_modifiers()");
        auto self = static_cast<const WaylandInputManager*>(data);

        xkb_state_update_mask(self->get_kb_state(),
               depressed, latched, locked, 0, 0, group);
    }

    void WaylandInputManager::keyboard_repeat(void *data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) 
    {
        LOG_DEBUG("keyboard_repeat()");
    }

    void WaylandInputManager::set_keyboard_active_window(Window *window) 
    {
        this->keyboard_active_window = window;
    }

    void WaylandInputManager::set_pointer_active_window(Window *window) 
    {
        this->pointer_active_window = window;
    }

    void WaylandInputManager::unset_keyboard_active_window() 
    {
        this->keyboard_active_window = nullptr;
    }

    void WaylandInputManager::unset_pointer_active_window() 
    {
        this->pointer_active_window = nullptr;
    }

}