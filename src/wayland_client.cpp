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
#include "wayland_input_manager.hpp"

struct wl_interface;

namespace tobi_engine
{

    WaylandClient::WaylandClient()
        :   display(std::make_unique<WaylandDisplay>()), 
            wayland_registry(std::make_unique<WaylandRegistry>(display->get())),
            wayland_input_manager(std::make_unique<WaylandInputManager>(wayland_registry.get()))
    {
        LOG_DEBUG("Constructing Client");
        initialize();
    }

    void WaylandClient::shell_ping(void *data, xdg_wm_base *shell, uint32_t serial) 
    {
        LOG_DEBUG("shell_ping()");
        auto self = static_cast<WaylandClient*>(data);
        if (!self)
        {
            LOG_ERROR("WaylandClient instance is null in shell_ping");
            return;
        }
        xdg_wm_base_pong(shell, serial);
    }

    bool WaylandClient::flush()
    {
        if (!display->flush())
        {
            LOG_ERROR("Failed to flush Wayland display");
            return false;
        }
        if (!display->roundtrip())
        {
            LOG_ERROR("Failed to roundtrip Wayland display");
            return false;
        }
        return true;
    }

    bool WaylandClient::update()
    {
        if (!display->dispatch())
        {
            LOG_ERROR("Failed to dispatch Wayland display");
            return false;
        }
        return true;
    }

    void WaylandClient::clear()
    {
        display->dispatch_pending();
        if (!display->roundtrip())
        {
            LOG_WARNING("Failed to roundtrip Wayland display");
        }
    }

    void WaylandClient::initialize()
    {
        LOG_DEBUG("initilizing Wayland Client");
        
        if(auto shell = wayland_registry->get_shell(); shell)
        {
            LOG_DEBUG("Wayland shell is available");
        
            static constexpr xdg_wm_base_listener shell_listener
            {
                &WaylandClient::shell_ping
            };
            xdg_wm_base_add_listener(shell, &shell_listener, this);
        }
        else
        {
            LOG_ERROR("Wayland shell is not available");
            throw std::runtime_error("Failed to initialize Wayland Client");
        }

    }

    auto WaylandClient::get_compositor() -> wl_compositor* const
    {
        return wayland_registry->get_compositor();
    }

    auto WaylandClient::get_subcompositor() -> wl_subcompositor* const
    {
        return wayland_registry->get_subcompositor();
    }
    auto WaylandClient::get_shell() -> xdg_wm_base* const
    {
      return wayland_registry->get_shell();
    }
    auto WaylandClient::get_shm() -> wl_shm* const
    {
        return wayland_registry->get_shm();
    }
    auto WaylandClient::get_input_manager() -> WaylandInputManager* 
    {
        return wayland_input_manager.get();
    }

} // namespace tobi_engine
