#pragma once

#include "wayland_display.hpp"
#include "wayland_registry.hpp"
#include "wayland_input_manager.hpp"

#include <wayland-client-protocol.h>
#include <memory>

namespace tobi_engine
{

    class WaylandClient
    {
    public:
        
        WaylandClient();
        ~WaylandClient() = default;

        auto get_compositor() -> wl_compositor* const;
        auto get_subcompositor() -> wl_subcompositor* const;
        auto get_shell() -> xdg_wm_base* const;
        auto get_shm() -> wl_shm* const;
        auto get_input_manager() -> WaylandInputManager* const;

        auto flush() -> bool;
        auto update() -> bool;
        void clear();

    private:

        void initialize();

        static void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial);

        std::unique_ptr<WaylandDisplay> display;
        std::unique_ptr<WaylandRegistry> wayland_registry;
        std::unique_ptr<WaylandInputManager> wayland_input_manager;

    };

} // namespace tobi_engine
