#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace tobi_engine
{
    struct WindowProperties
    {
        uint32_t width;
        uint32_t height;
        std::string title;
    };

    class Window
    {
    public:
        explicit Window(const WindowProperties &properties);
        Window(Window &&) = default;
        Window(const Window &) = default;
        Window &operator=(Window &&) = default;
        Window &operator=(const Window &) = default;
        virtual ~Window() = default;

        virtual void update() = 0;
        virtual bool should_close() = 0;
        virtual void on_key(uint32_t key, uint32_t state) = 0;
        virtual void on_pointer_button(uint32_t button, uint32_t state) = 0;

        virtual void on_pointer_motion(int32_t x, int32_t y) = 0;

        uint64_t get_uid() { return uid; }
    
    protected:
        WindowProperties properties;
        uint64_t uid;

        virtual void initialize() = 0;
        
    };
}
