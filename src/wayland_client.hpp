#pragma once

#include "wayland_input_manager.hpp"
#include "wayland_types.hpp"
#include "wayland_window.hpp"
#include "wayland_display.hpp"
#include "wayland_registry.hpp"
#include <memory>
#include <optional>
#include <wayland-client-protocol.h>

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

        auto get_compositor() -> wl_compositor* const;
        auto get_subcompositor() -> wl_subcompositor* const;
        auto get_shell() -> xdg_wm_base* const;
        auto get_shm() -> wl_shm* const;

        auto get_input_manager() -> WaylandInputManager*;

        bool flush();
        bool update();
        void clear();

    private:
        WaylandClient();

        static void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial);

        void initialize();

        std::unique_ptr<WaylandDisplay> display;
        std::unique_ptr<WaylandRegistry> wayland_registry;
        std::unique_ptr<WaylandInputManager> wayland_input_manager;

};

}