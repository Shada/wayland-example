#include "wayland_client.hpp"

#include <cstddef>
#include <memory>
#include <stdexcept>

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"

#include "utils/logger.hpp"
#include "window_registry.hpp"
#include "wayland_window.hpp"

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

    void keyboard_map(void *data, struct wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size) 
    {
        printf("keyboard_map()\n");
    }

    void keyboard_enter(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array* keys) 
    {
        printf("keyboard_enter()\n");
        auto window_registry = WindowRegistry::get_instance();
        window_registry->set_active_window(surface);

    }

    void keyboard_leave(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface) 
    {
        printf("keyboard_leave()\n");
    }

    void keyboard_key(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t t, uint32_t key, uint32_t status) 
    {
        auto window_registry = WindowRegistry::get_instance();
        printf("keyboard_key() %d\n", key);
        
        window_registry->on_keypress(key);

        /*if(key == 30)
            xdg_toplevel_set_fullscreen(x_top, NULL);
        if(key == 32)
            xdg_toplevel_unset_fullscreen(x_top);*/
    }

    void keyboard_modifiers(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t group) 
    {
        printf("keyboard_modifiers()\n");
    }

    void keyboard_repeat(void *data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) 
    {
        printf("keyboard_repeat()\n");
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
    static void seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) 
    {
        Logger::debug("seat_capabilities()");
        auto client = static_cast<WaylandClient*>(data);

        if(capabilities & WL_SEAT_CAPABILITY_KEYBOARD && !client->get_keyboard()) 
        {
            auto keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(keyboard, &keyboard_listener, client);
            client->set_keyboard(keyboard);
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

    void registry_global(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) 
    {
        auto client = static_cast<WaylandClient*>(data);
        
        auto interface_name = static_cast<std::string>(interface);

        if (interface_name.compare(wl_compositor_interface.name) == 0) 
        {
            const uint32_t client_supported_version = 6;
            const uint32_t bind_version = std::min(version, client_supported_version);
            if (bind_version < client_supported_version) 
            {
                throw std::runtime_error("Client supports xdg_wm_base version " + std::to_string(client_supported_version) +
                            ", but server only supports up to " + std::to_string(version));
            }
            auto compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, bind_version));
            client->set_compositor(compositor);
            // Debug log for compositor registration
            Logger::debug("added compositor to registry");
        }
        else if (interface_name.compare(wl_shm_interface.name) == 0) 
        {
            const uint32_t client_supported_version = 2;
            const uint32_t bind_version = std::min(version, client_supported_version);
            if (bind_version < client_supported_version) 
            {
                throw std::runtime_error("Client supports xdg_wm_base version " + std::to_string(client_supported_version) +
                            ", but server only supports up to " + std::to_string(version));
            }
            auto shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, bind_version));
            client->set_shm(shm);
            Logger::debug("added shm to registry");
        }
        else if (interface_name.compare(xdg_wm_base_interface.name) == 0) 
        {
            const uint32_t client_supported_version = 6;
            const uint32_t bind_version = std::min(version, client_supported_version);
            if (bind_version < client_supported_version) 
            {
                throw std::runtime_error("Client supports xdg_wm_base version " + std::to_string(client_supported_version) +
                            ", but server only supports up to " + std::to_string(version));
            }
            auto shell = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, bind_version));
            xdg_wm_base_add_listener(shell, &shell_listener, client);
            client->set_shell(shell);
            Logger::debug("added shell to registry");
        }
        else if (interface_name.compare(wl_seat_interface.name) == 0) 
        {
            const uint32_t client_supported_version = 8;
            const uint32_t bind_version = std::min(version, client_supported_version);
            if (bind_version < client_supported_version) 
            {
                throw std::runtime_error("Client supports xdg_wm_base version " + std::to_string(client_supported_version) +
                            ", but server only supports up to " + std::to_string(version));
            }
            auto seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, bind_version));
            wl_seat_add_listener(seat, &seat_listener, client);
            client->set_seat(seat);
            Logger::debug("added seat to registry");
        }
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
        wl_display_flush(display);
        
        if(keyboard) 
        {
            wl_keyboard_destroy(keyboard);
            keyboard = nullptr;
        }
        if(seat) 
        {
            wl_seat_destroy(seat);
            seat = nullptr;
        }
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
            shm(nullptr),
            seat(nullptr),
            keyboard(nullptr)
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
            throw std::runtime_error("Failed to create Wayland Registry!");

        wl_registry_add_listener(registry, &registry_listener, this);

        // Round-trip to synchronize with the server
        wl_display_roundtrip(display);
    }
    
}
