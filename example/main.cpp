#include <cstddef>
#include <cstdint>
#include <memory>

#include "window_registry.hpp"

int main() {
    // Initialize Wayland display
    tobi_engine::WindowProperties properties = {400,400,"test"};

    auto registry = tobi_engine::WindowRegistry::get_instance();
    auto window = registry->create_window(properties);
    uint32_t c = 200;
    
    while(!window->should_close())
        window->update();

    return 0;
}