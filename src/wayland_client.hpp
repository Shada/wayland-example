#pragma once

#include "wayland_input_manager.hpp"
#include "wayland_types.hpp"
#include "wayland_window.hpp"
#include "wayland_display.hpp"
#include "wayland_registry.hpp"
#include <memory>

namespace tobi_engine
{

class WaylandClient
{
    public:

        static WaylandClient& get_instance()
        {
            static WaylandClient instance;
            return instance;
        }
        
        ~WaylandClient() = default;
        
        wl_compositor* get_compositor() const { return compositor; }
        wl_subcompositor* get_subcompositor() const { return subcompositor; }
        xdg_wm_base* get_shell() const { return shell; }
        wl_shm* get_shm() const { return shm; }

        // TODO: should probably be moved to WaylandInputManager
        bool is_keyboard_available() const { return wayland_input_manager->get_keyboard() != nullptr; }
        wl_pointer* get_pointer() const { return wayland_input_manager->get_pointer(); }
        xkb_state* get_state() const { return wayland_input_manager->get_kb_state(); }
        
        // TODO: have client hold the window registry
        std::shared_ptr<WaylandWindow> create_window(WindowProperties properties)
        {
            return std::make_shared<WaylandWindow>(properties);
        }
        
        bool flush();
        bool update();
        void clear();

    private:

        static void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial);

        static constexpr xdg_wm_base_listener shell_listener
        {
            &WaylandClient::shell_ping
        };

        void on_shell_ping(xdg_wm_base *shell, uint32_t serial);

        WaylandClient();

        void initialize();

        std::unique_ptr<WaylandDisplay> display;
        std::unique_ptr<WaylandRegistry> wayland_registry;
        std::unique_ptr<WaylandInputManager> wayland_input_manager;

        wl_compositor* compositor;
        wl_subcompositor* subcompositor;
        xdg_wm_base* shell;
        wl_shm* shm;

};

}