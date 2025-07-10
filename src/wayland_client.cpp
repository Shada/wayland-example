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
    void* bind_to_registry(wl_registry *registry, uint32_t name, const wl_interface* interface, uint32_t client_version, uint32_t server_version)
    {
        LOG_DEBUG("Adding {} to registry", interface->name);

        const uint32_t bind_version = std::min(server_version, client_version);
        
        if (bind_version < client_version) 
        {
            LOG_ERROR("Client supports {} version {}, but server only supports up to version {}.", 
                interface->name, client_version, server_version);
            return nullptr;
        }
        else if(bind_version < server_version)
        {
            LOG_DEBUG("Server supports {} version {}, but client only supports up to {}",
                interface->name, server_version, client_version);
        }

        return wl_registry_bind(registry, name, interface, bind_version);
    }
    static const std::unordered_map<std::string_view, std::function<void(wl_registry*, uint32_t, uint32_t, WaylandClient*)>> registry_handlers = 
    {
        { wl_compositor_interface.name, [](wl_registry* r, uint32_t n, uint32_t v, WaylandClient* client) 
        {
            client->set_compositor(static_cast<wl_compositor*>(bind_to_registry(r, n, &wl_compositor_interface, 6, v)));
        }},
        { wl_subcompositor_interface.name, [](wl_registry* r, uint32_t n, uint32_t v, WaylandClient* client) 
        {
            client->set_subcompositor(static_cast<wl_subcompositor*>(bind_to_registry(r, n, &wl_subcompositor_interface, 1, v)));
        }},
        { wl_shm_interface.name, [](wl_registry* r, uint32_t n, uint32_t v, WaylandClient* client) 
        {
            client->set_shm(static_cast<wl_shm*>(bind_to_registry(r, n, &wl_shm_interface, 2, v)));
        }},
        { xdg_wm_base_interface.name, [](wl_registry* r, uint32_t n, uint32_t v, WaylandClient* client) 
        {
            client->set_shell(static_cast<xdg_wm_base*>(bind_to_registry(r, n, &xdg_wm_base_interface, 6, v)));
        }},
        { wl_seat_interface.name, [](wl_registry* r, uint32_t n, uint32_t v, WaylandClient* client) 
        {
            client->set_seat(static_cast<wl_seat*>(bind_to_registry(r, n, &wl_seat_interface, 4, v)));
        }},
    };

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

    void WaylandClient::registry_global_add(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) 
    {
        auto self = static_cast<WaylandClient*>(data);
        self->on_registry_global_add(registry, name, interface, version);
    }

    void WaylandClient::on_registry_global_add(wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
    {
        auto it = registry_handlers.find(interface);
        if (it != registry_handlers.end())
            it->second(registry, name, version, this);
    }

    void WaylandClient::registry_global_remove(void *data, wl_registry *registry, uint32_t name) 
    {
        // This function will be called when a global object is removed from the registry
        LOG_DEBUG("Global object removed: {}", name);
    }

    WaylandClient::WaylandClient()
    {
        initialize();
    }

    bool WaylandClient::flush()
    {
        if (wl_display_flush(display.get()) == -1)
        {
            LOG_ERROR("Failed to flush Wayland display: {}", wl_display_get_error(display.get()));
            return false;
        }
        if (wl_display_roundtrip(display.get()) == -1)
        {
            LOG_ERROR("Failed to roundtrip Wayland display: {}", wl_display_get_error(display.get()));
            return false;
        }
        
        return true;
    }

    bool WaylandClient::update()
    {
        if (wl_display_dispatch(display.get()) == -1)
        {
            LOG_ERROR("Failed to dispatch Wayland display: {}", wl_display_get_error(display.get()));
            return false;
        }
        return true;
    }
    void WaylandClient::clear()
    {
        wl_display_dispatch_pending(display.get());
        wl_display_roundtrip(display.get());
    }

    void WaylandClient::initialize()
    {
        LOG_DEBUG("initilizing Wayland Client");
        display = WlDisplayPtr(wl_display_connect(nullptr));
        if(!display)
            throw std::runtime_error("Failed to create Wayland Display!");

        registry = WlRegistryPtr(wl_display_get_registry(display.get()));
        if(!registry)
            throw std::runtime_error("Failed to create Wayland Registry!");

        wl_registry_add_listener(registry.get(), &registry_listener, this);

        // Round-trip to synchronize with the server
        wl_display_roundtrip(display.get());

        kb_context = XkbContextPtr(xkb_context_new(XKB_CONTEXT_NO_FLAGS));

    }
    
}
