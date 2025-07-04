#pragma once

#include <memory>
#include <unordered_map>

#include "window.hpp"

namespace tobi_engine 
{
    
class WindowRegistry 
{
public:
    static std::shared_ptr<WindowRegistry> get_instance();

    virtual ~WindowRegistry() = default;

    WindowRegistry(const WindowRegistry&) = delete;
    WindowRegistry& operator=(const WindowRegistry&) = delete;

    std::shared_ptr<Window> create_window(WindowProperties properties);
    void set_active_window(uint64_t uid) 
    {
        if(windows.contains(uid)) 
            keyboard_active_window = windows[uid]; 
    }
    void set_pointer_active_window(uint64_t uid) 
    {
        if(windows.contains(uid)) 
            pointer_active_window = windows[uid]; 
    }
    void unset_active_window() { keyboard_active_window = nullptr; }
    void unset_pointer_active_window() { pointer_active_window = nullptr; }

    void on_key(uint32_t key, uint32_t state);
    void on_pointer_leave() 
    { // do things like unset cursor, etc.
        pointer_active_window = nullptr;
    };
    void on_pointer_button(uint32_t button, uint32_t state);
    void on_pointer_motion(int32_t x, int32_t y);

private:

    static std::shared_ptr<WindowRegistry> instance;

    WindowRegistry() = default;
    
    std::unordered_map<uint64_t, std::shared_ptr<Window>> windows;
    std::shared_ptr<Window> keyboard_active_window = nullptr;
    std::shared_ptr<Window> pointer_active_window = nullptr;

};

} // namespace wl
