#pragma once

#include "window.hpp"
#include <memory>
#include <unordered_map>

namespace tobi_engine 
{
    
class WindowRegistry 
{
public:
    static std::shared_ptr<WindowRegistry> get_instance();

    virtual ~WindowRegistry();

    WindowRegistry(const WindowRegistry&) = delete;
    WindowRegistry& operator=(const WindowRegistry&) = delete;

    std::shared_ptr<Window> create_window(WindowProperties properties);
    void set_active_window(void* key) { active_window = windows[key]; }
    void on_keypress(uint32_t key);

private:

    static std::shared_ptr<WindowRegistry> instance;

    WindowRegistry();
    
    std::unordered_map<void*, std::shared_ptr<Window>> windows;
    std::shared_ptr<Window> active_window = nullptr;

};

} // namespace wl
