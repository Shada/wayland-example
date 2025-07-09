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
    void* bind_to_registry(wl_registry *registry, uint32_t name, const wl_interface* interface, uint32_t client_version, uint32_t server_version);

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

    void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial) 
    {
        xdg_wm_base_pong(shell, serial);
    }

    static constexpr xdg_wm_base_listener shell_listener = 
    {
        shell_ping
    };

    static void seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) 
    {
        LOG_DEBUG("seat_capabilities() = {}", capabilities);
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
            client->unset_keyboard();
        }

        bool is_pointer_supported = capabilities & WL_SEAT_CAPABILITY_POINTER;
        if(is_pointer_supported && !client->get_pointer())
        {
            auto pointer = PointerPtr(wl_seat_get_pointer(seat));
            wl_pointer_add_listener(pointer.get(), &pointer_listener, client);
            client->set_pointer(std::move(pointer));
        }
        else if(!is_pointer_supported && client->get_pointer())
        {
            client->unset_pointer();
        }
    }

    static void seat_name(void* data, struct wl_seat* seat, const char* name) 
    {
        LOG_DEBUG("seat_name()");
    }

    static constexpr wl_seat_listener seat_listener = 
    {
        .capabilities = seat_capabilities,
        .name = seat_name
    };

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
    
    void WaylandClient::set_seat(wl_seat* seat)
    {
        if(!seat)
        {
            LOG_ERROR("Failed to set seat: seat is null");
            return;
        }
        wl_seat_add_listener(seat, &seat_listener, this);
        this->seat = SeatPtr(seat);
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
        LOG_DEBUG("Global object removed: {}", name);
    }

    const struct wl_registry_listener registry_listener = 
    {
        .global = registry_global,
        .global_remove = registry_global_remove
    };

    std::shared_ptr<WaylandClient> WaylandClient::instance = nullptr;

    std::mutex instance_mutex;
    std::shared_ptr<WaylandClient> WaylandClient::get_instance()
    {
        std::lock_guard<std::mutex> lock(instance_mutex);
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

    bool WaylandClient::flush()
    {
        auto ret = wl_display_flush(display.get());
        wl_display_roundtrip(display.get());
        
        return ret != -1;
    }

    bool WaylandClient::update()
    {
        auto ret = wl_display_dispatch(display.get());
        return ret != -1;
    }
    void WaylandClient::clear()
    {
        wl_display_dispatch_pending(display.get());
        wl_display_roundtrip(display.get());
    }

    void WaylandClient::initialize()
    {
        LOG_DEBUG("initilizing Wayland Client");
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
