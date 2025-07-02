#include "wayland_client.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>

#include <string>
#include <unordered_map>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include "wayland-xdg-shell-client-protocol.h"
#include "wayland-xdg-decoration-unstable-v1-client-protocol.h"
#include <xkbcommon/xkbcommon.h>

#include "utils/logger.hpp"
#include "wayland_deleters.hpp"
#include "window_registry.hpp"
#include "wayland_window.hpp"
#include "wayland_input.hpp"

struct wl_interface;

namespace tobi_engine
{
    void* bind_to_registry(wl_registry *registry, uint32_t name, const wl_interface* interface, uint32_t client_version, uint32_t server_version);

    static const std::unordered_map<std::string, std::function<void(wl_registry*, uint32_t, uint32_t, WaylandClient*)>> registry_handlers = 
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

    void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial) 
    {
        xdg_wm_base_pong(shell, serial);
    }

    const struct xdg_wm_base_listener shell_listener = 
    {
        shell_ping
    };

    static void seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) 
    {
        Logger::debug("seat_capabilities() = " + std::to_string(capabilities));
        auto client = static_cast<WaylandClient*>(data);

        bool is_keyboard_supported = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;
        if(is_keyboard_supported && !client->is_keyboard_available()) 
        {
            auto keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(keyboard, &keyboard_listener, client);
            client->set_keyboard(keyboard);
        }
        else if(!is_keyboard_supported && client->is_keyboard_available()) 
        {
            client->set_keyboard(nullptr);
        }

        bool is_pointer_supported = capabilities & WL_SEAT_CAPABILITY_POINTER;
        if(is_pointer_supported && !client->get_pointer())
        {
            auto pointer = wl_seat_get_pointer(seat);
            wl_pointer_add_listener(pointer, &pointer_listener, client);
            client->set_pointer(pointer);
        }
        else if(!is_pointer_supported && client->get_pointer())
        {
            client->set_pointer(nullptr);
        }
    }

    static void seat_name(void* data, struct wl_seat* seat, const char* name) 
    {
        Logger::debug("seat_name()");
    }

    const struct wl_seat_listener seat_listener = 
    {
        .capabilities = seat_capabilities,
        .name = seat_name
    };

    void* bind_to_registry(wl_registry *registry, uint32_t name, const wl_interface* interface, uint32_t client_version, uint32_t server_version)
    {
        Logger::debug("Adding " + std::string(interface->name) + " to registry");

        const uint32_t bind_version = std::min(server_version, client_version);
        
        if (bind_version < client_version) 
        {
            throw std::runtime_error("Client supports " + std::string(interface->name) + " version " + std::to_string(client_version) +
                        ", but server only supports up to " + std::to_string(server_version));
        }
        else if(bind_version < server_version)
        {
            Logger::debug( "Server supports " + std::string(interface->name) + " version " + std::to_string(server_version)
                + ", but client only supports up to " + std::to_string(client_version));
        }

        return wl_registry_bind(registry, name, interface, bind_version);
    }
    
    void WaylandClient::set_seat(wl_seat* seat)
    {
        wl_seat_add_listener(seat, &seat_listener, this);
        this->seat = SeatPtr(seat);
    }

    void WaylandClient::set_shell(xdg_wm_base* shell)
    {
        xdg_wm_base_add_listener(shell, &shell_listener, this);
        this->shell = XdgShellPtr(shell); 
    }

    void registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) 
    {
        auto client = static_cast<WaylandClient*>(data);
        auto it = registry_handlers.find(interface);

        if (it != registry_handlers.end()) 
            it->second(registry, name, version, client);
    }

    void registry_global_remove(void *data, wl_registry *registry, uint32_t name) 
    {
        // This function will be called when a global object is removed from the registry
        Logger::debug("Global object removed: name " + std::to_string(name));
    }

    const struct wl_registry_listener registry_listener = 
    {
        .global = registry_global,
        .global_remove = registry_global_remove
    };

    std::shared_ptr<WaylandClient> WaylandClient::instance = nullptr;

    std::shared_ptr<WaylandClient> WaylandClient::get_instance()
    {
        if(!instance)
            instance = std::shared_ptr<WaylandClient>(new WaylandClient());
        return instance;
    }

    WaylandClient::~WaylandClient() 
    {
        if(display)
            wl_display_flush(display.get());
    }

    WaylandClient::WaylandClient()
        :   display(nullptr),
            registry(nullptr),
            compositor(nullptr),
            subcompositor(nullptr),
            shell(nullptr),
            shm(nullptr),
            seat(nullptr),
            keyboard(nullptr),
            kb_context(nullptr),
            kb_state(nullptr)
    {
        initialize();
    }

    void WaylandClient::flush()
    {
        wl_display_flush(display.get());
        wl_display_roundtrip(display.get());
    }

    void WaylandClient::update()
    {
        wl_display_dispatch(display.get());
    }
    void WaylandClient::clear()
    {
        wl_display_dispatch_pending(display.get());
        wl_display_roundtrip(display.get());
    }

    void WaylandClient::initialize()
    {
        display = DisplayPtr(wl_display_connect(nullptr));
        if(!display)
            throw std::runtime_error("Failed to create Wayland Display!");

        registry = RegistryPtr(wl_display_get_registry(display.get()));
        if(!registry)
            throw std::runtime_error("Failed to create Wayland Registry!");

        wl_registry_add_listener(registry.get(), &registry_listener, this);

        // Round-trip to synchronize with the server
        wl_display_roundtrip(display.get());

        kb_context = KbContextPtr(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
    }
    
}
