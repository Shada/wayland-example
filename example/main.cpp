#include "window_manager.hpp"

#include <cstdint>
#include <cstdio>
#include <memory>

int main() {
    // Initialize Wayland display
    tobi_engine::WindowProperties properties = {400,400,"test"};

    auto window_manager = std::make_unique<tobi_engine::WindowManager>();
    auto window = window_manager->create_window(properties);

    uint32_t c = 200;
    
    while(!window->should_close())
    {
        window->update();
    }
        

    return 0;
}