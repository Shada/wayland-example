#pragma once

#include "window.hpp"

#include <memory>

namespace tobi_engine
{

    class WindowRegistry;

    class WindowManager
    {
    public:

        WindowManager();
        ~WindowManager() = default;

        std::shared_ptr<Window> create_window(const WindowProperties& properties);

    private:

        std::shared_ptr<WindowRegistry> window_registry;

    };

} // namespace tobi_engine
