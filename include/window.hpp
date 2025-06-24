#pragma once

#include <memory>

namespace tobi_engine
{
    
    class Window
    {
    public:
        Window() = default;
        Window(Window &&) = default;
        Window(const Window &) = default;
        Window &operator=(Window &&) = default;
        Window &operator=(const Window &) = default;
        virtual ~Window() = default;

        virtual void update() = 0;
        virtual bool should_close() = 0;
        virtual void on_keypress(uint32_t key) = 0;
    
    private:
        
    };
    std::shared_ptr<Window> create_window();
    
}
