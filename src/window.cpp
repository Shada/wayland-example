#include "window.hpp"
#include "wayland_window.hpp"
#include <memory>

namespace tobi_engine
{
    std::shared_ptr<Window> create_window()
    {
        return std::make_shared<MyWindow>();
    }   
}