#include "window_registry.hpp"

#include "utils/logger.hpp"
#include "wayland_client.hpp"
#include "wayland_window.hpp"

#include <memory>

namespace tobi_engine
{

    WindowRegistry::WindowRegistry()
    {
        LOG_DEBUG("WindowRegistry initialized");
        client = std::make_unique<WaylandClient>();
    }

    auto WindowRegistry::create_window(const WindowProperties& properties) -> std::shared_ptr<Window>
    {
        auto window = std::make_shared<WaylandWindow>(properties, client.get());
        windows[window->get_uid()] = window;
        
        return window;
    }
    
} // namespace tobi_engine
