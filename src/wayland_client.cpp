#include "wayland_client.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"

#include "utils/logger.hpp"

void shell_ping(void *data, struct xdg_wm_base *shell, uint32_t serial) 
{
    xdg_wm_base_pong(shell, serial);
}
const struct xdg_wm_base_listener shell_listener = 
{
    .ping = shell_ping
};

namespace tobi_engine
{

    void registry_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) 
    {
        auto window = (WaylandClient*)data;
        if (std::strcmp(interface, wl_compositor_interface.name) == 0) 
        {
            auto compositor = (struct wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, 4);
            window->set_compositor(compositor);
            // Debug log for compositor registration
            Logger::debug("added compositor to registry");
        }
        else if (std::strcmp(interface, wl_shm_interface.name) == 0) 
        {
            auto shm = (struct wl_shm*)wl_registry_bind(registry, name, &wl_shm_interface, 1);
            window->set_shm(shm);
            Logger::debug("added shm to registry");
        }
        else if (std::strcmp(interface, xdg_wm_base_interface.name) == 0) 
        {
            auto shell = (struct xdg_wm_base*)wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
            xdg_wm_base_add_listener(shell, &shell_listener, 0);
            window->set_shell(shell);
            Logger::debug("added shell to registry");
        }
        //else if (!strcmp(interface, wl_seat_interface.name)) {
        //    seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
        //    wl_seat_add_listener(seat, &seat_listener, 0);
    // }
    }

    void registry_global_remove(void *data, struct wl_registry *registry, uint32_t name) {

        // This function will be called when a global object is removed from the registry
        Logger::debug("Global object removed: name " + std::to_string(name));
    }
    static const struct wl_registry_listener registry_listener = 
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
        wl_display_flush(display);
        
        if(shm) wl_shm_release(shm);
        if(shell) xdg_wm_base_destroy(shell);
        if(compositor) wl_compositor_destroy(compositor);
        if(registry) wl_registry_destroy(registry);
        if(display) wl_display_disconnect(display);
    }

    WaylandClient::WaylandClient()
    {
        initialize();
    }

    void WaylandClient::update()
    {
        wl_display_dispatch(display);
    }

    void WaylandClient::initialize()
    {
        display = wl_display_connect(nullptr);
        if(!display)
            throw std::runtime_error("Failed to create Wayland Display!");

        registry = wl_display_get_registry(display);
        if(!registry)
            throw std::runtime_error("@Failed to create Wayland Registry!");

        wl_registry_add_listener(registry, &registry_listener, this);

        // Round-trip to synchronize with the server
        wl_display_roundtrip(display);
    }
    
}
