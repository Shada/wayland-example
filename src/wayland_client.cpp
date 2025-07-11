#include "wayland_client.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <format>

#include <string>
#include <unordered_map>
#include <wayland-client-core.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>
#include <wayland-util.h>
#include "wayland-xdg-shell-client-protocol.h"
#include "wayland-xdg-decoration-unstable-v1-client-protocol.h"
#include <xkbcommon/xkbcommon.h>

#include "utils/logger.hpp"
#include "wayland_types.hpp"
#include "window_registry.hpp"
#include "wayland_window.hpp"
#include "wayland_input.hpp"

struct wl_interface;

namespace tobi_engine
{

    WaylandClient::WaylandClient()
        : wayland_registry(std::make_unique<WaylandRegistry>(display))
    {
        if (!display.roundtrip()) // Temporary
        {
            LOG_ERROR("Failed to roundtrip Wayland display during initialization!");
            throw std::runtime_error("Failed to initialize Wayland Client");
        }
        compositor = WlCompositorPtr(static_cast<wl_compositor*>(wayland_registry->bind_interface(wl_compositor_interface.name, &wl_compositor_interface, 6)));
        subcompositor = WlSubCompositorPtr(static_cast<wl_subcompositor*>(wayland_registry->bind_interface(wl_subcompositor_interface.name, &wl_subcompositor_interface, 1)));
        shm = WlShmPtr(static_cast<wl_shm*>(wayland_registry->bind_interface(wl_shm_interface.name, &wl_shm_interface, 2)));
        set_shell(static_cast<xdg_wm_base*>(wayland_registry->bind_interface(xdg_wm_base_interface.name, &xdg_wm_base_interface, 6)));
        set_seat(static_cast<wl_seat*>(wayland_registry->bind_interface(wl_seat_interface.name, &wl_seat_interface, 4)));

        initialize();
    }

    void WaylandClient::shell_ping(void *data, xdg_wm_base *shell, uint32_t serial) 
    {
        auto client = static_cast<WaylandClient*>(data);
        client->on_shell_ping(shell, serial);
    }

    void WaylandClient::on_shell_ping(xdg_wm_base *shell, uint32_t serial) 
    {
        xdg_wm_base_pong(shell, serial);
    }

    void WaylandClient::seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) 
    {
        LOG_DEBUG("seat_capabilities() = {}", capabilities);
        auto client = static_cast<WaylandClient*>(data);
        client->on_seat_capabilities(seat, capabilities);
    }

    void WaylandClient::on_seat_capabilities(struct wl_seat* seat, uint32_t capabilities)
    {
        if (capabilities & WL_SEAT_CAPABILITY_POINTER) 
        {
            if (!pointer) 
            {
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

    void WaylandClient::seat_name(void* data, struct wl_seat* seat, const char* name) 
    {
        LOG_DEBUG("seat_name()");
        auto self = static_cast<WaylandClient*>(data);
        self->on_seat_name(seat, name);
    }
    void WaylandClient::on_seat_name(struct wl_seat* seat, const char* name)
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

    
    void WaylandClient::set_seat(wl_seat* seat)
    {
        if(!seat)
        {
            LOG_ERROR("Failed to set seat: seat is null");
            return;
        }
        wl_seat_add_listener(seat, &seat_listener, this);
        this->seat = WlSeatPtr(seat);
    }

    void WaylandClient::set_shell(xdg_wm_base* shell)
    {
        if(!shell)
        {
            LOG_ERROR("Failed to set shell: shell is null");
            return;
        }
        xdg_wm_base_add_listener(shell, &shell_listener, this);
        this->shell = XdgShellPtr(shell); 
    }

    bool WaylandClient::flush()
    {
        if (!display.flush())
        {
            LOG_ERROR("Failed to flush Wayland display");
            return false;
        }
        if (!display.roundtrip())
        {
            LOG_ERROR("Failed to roundtrip Wayland display");
            return false;
        }
        return true;
    }

    bool WaylandClient::update()
    {
        if (!display.dispatch())
        {
            LOG_ERROR("Failed to dispatch Wayland display");
            return false;
        }
        return true;
    }
    void WaylandClient::clear()
    {
        display.dispatch_pending();
        if (!display.roundtrip())
        {
            LOG_WARNING("Failed to roundtrip Wayland display");
        }
    }

    void WaylandClient::initialize()
    {
        LOG_DEBUG("initilizing Wayland Client");
        //if (!display.roundtrip())
        //{
        //    LOG_ERROR("Failed to roundtrip Wayland display after initialization!");
        //    throw std::runtime_error("Failed to initialize Wayland Client");
        //}
        kb_context = XkbContextPtr(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
    }
    
}
