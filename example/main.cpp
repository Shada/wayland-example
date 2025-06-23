#include <cstddef>
#include <cstdint>
#include <memory>

#include "window.hpp"

int main() {
    // Initialize Wayland display

    auto window = tobi_engine::create_window();
    uint32_t c = 1000;
    
    for(size_t a = 0; a < c; a++)
        window->update();


    

    return 0;
}