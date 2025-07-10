#pragma once

#include "../wayland_types.hpp"

namespace tobi_engine 
{

class WaylandDisplay 
{
public:
    WaylandDisplay();
    wl_display* get() const { return display.get(); }
    bool flush() 
    {
        return wl_display_flush(display.get()) != -1;
    }
    bool dispatch() 
    {
        return wl_display_dispatch(display.get()) != -1;
    }
    bool roundtrip() 
    {
        return wl_display_roundtrip(display.get()) != -1;
    }
    void dispatch_pending() 
    {
        wl_display_dispatch_pending(display.get());
    }
private:
    WlDisplayPtr display;
};

} // namespace tobi_engine
