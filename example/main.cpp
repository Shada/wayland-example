#include <cstddef>
#include <cstdint>
#include <memory>

#include "window.hpp"

int main() {
    // Initialize Wayland display

    auto window = tobi_engine::create_window();
    uint8_t c = 200;
    
    for(size_t a = 0; a < 100; a++)
        window->update();


    

    return 0;
}