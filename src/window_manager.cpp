#include "window_manager.hpp"

#include "window_registry.hpp"
#include "utils/logger.hpp"

namespace tobi_engine
{

    WindowManager::WindowManager()
        : window_registry(std::make_shared<WindowRegistry>())
    {
        LOG_DEBUG("WindowManager initialized");
    }

    std::shared_ptr<Window> WindowManager::create_window(const WindowProperties& properties)
    {
        LOG_DEBUG("Creating window");
        return window_registry->create_window(properties);
    }

} // namespace tobi_engine
