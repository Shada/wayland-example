#pragma once

#include <memory>
#include <unordered_map>

#include "window.hpp"

namespace tobi_engine 
{
    
class WindowRegistry 
{
public:

    static WindowRegistry& get_instance();

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
    void unset_active_window() { keyboard_active_window.reset(); }
    void unset_pointer_active_window() { pointer_active_window.reset(); }

    void on_key(uint32_t key, uint32_t state);
    void on_pointer_leave() { pointer_active_window.reset(); };
    void on_pointer_button(uint32_t button, uint32_t state);
    void on_pointer_motion(int32_t x, int32_t y);

private:

    WindowRegistry();
    
    std::unordered_map<uint64_t, std::shared_ptr<Window>> windows;
    std::weak_ptr<Window> keyboard_active_window;
    std::weak_ptr<Window> pointer_active_window;

};

} // namespace tobi_engine
