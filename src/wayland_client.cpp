#include "wayland_client.hpp"

#include <cstddef>
#include <stdexcept>

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"

#include "utils/logger.hpp"

namespace tobi_engine
{

    void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial) 
    {
        xdg_wm_base_pong(shell, serial);
    }
    const struct xdg_wm_base_listener shell_listener = 
    {
        shell_ping
    };

    void registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) 
    {
        auto window = (WaylandClient*)data;
        
        auto interface_name = std::string(interface);

        if (interface_name.compare(wl_compositor_interface.name) == 0) 
        {
            const uint32_t client_supported_version = 6;
            const uint32_t bind_version = std::min(version, client_supported_version);
            if (bind_version < client_supported_version) 
            {
                throw std::runtime_error("Server supports wl_compositor version " + std::to_string(version) +
                            ", but client only supports up to " + std::to_string(client_supported_version));
            }
            auto compositor = (wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, bind_version);
            window->set_compositor(compositor);
            // Debug log for compositor registration
            Logger::debug("added compositor to registry");
        }
        else if (interface_name.compare(wl_shm_interface.name) == 0) 
        {
            const uint32_t client_supported_version = 2;
            const uint32_t bind_version = std::min(version, client_supported_version);
            if (bind_version < client_supported_version) 
            {
                throw std::runtime_error("Server supports wl_shm version " + std::to_string(version) +
                            ", but client only supports up to " + std::to_string(client_supported_version));
            }
            auto shm = (wl_shm*)wl_registry_bind(registry, name, &wl_shm_interface, bind_version);
            window->set_shm(shm);
            Logger::debug("added shm to registry");
        }
        else if (interface_name.compare(xdg_wm_base_interface.name) == 0) 
        {
            const uint32_t client_supported_version = 5;
            const uint32_t bind_version = std::min(version, client_supported_version);
            if (bind_version < client_supported_version) 
            {
                throw std::runtime_error("Server supports xdg_wm_base version " + std::to_string(version) +
                            ", but client only supports up to " + std::to_string(client_supported_version));
            }
            auto shell = (xdg_wm_base*)wl_registry_bind(registry, name, &xdg_wm_base_interface, bind_version);
            xdg_wm_base_add_listener(shell, &shell_listener, window);
            window->set_shell(shell);
            Logger::debug("added shell to registry");
        }
        //else if (!strcmp(interface, wl_seat_interface.name)) 
        //{
            //const uint32_t bind_version = 1;
            //if (version < bind_version) 
            //{
            //    throw std::runtime_error("Wayland server does not support required wl_compositor version!");
            //}
        //    seat = wl_registry_bind(registry, name, &wl_seat_interface, bind_version);
        //    wl_seat_add_listener(seat, &seat_listener, 0);
    // }
    }

    void registry_global_remove(void *data, wl_registry *registry, uint32_t name) {

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
        wl_display_flush(display);
        
        if(shm) 
        {
            wl_shm_release(shm);
            shm = nullptr;
        }
        if(shell) 
        {
            xdg_wm_base_destroy(shell);
            shell = nullptr;
        }
        if(compositor) 
        {
            wl_compositor_destroy(compositor);
            compositor = nullptr;
        }
        if(registry) 
        {
            wl_registry_destroy(registry);
            registry = nullptr;
        }
        if(display) 
        {
            wl_display_disconnect(display);
            display = nullptr;
        }
    }

    WaylandClient::WaylandClient()
        :   display(nullptr),
            registry(nullptr),
            compositor(nullptr),
            shell(nullptr),
            shm(nullptr)
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
