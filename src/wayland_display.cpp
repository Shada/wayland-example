#include "wayland_display.hpp"

#include <stdexcept>

namespace tobi_engine
{

    WaylandDisplay::WaylandDisplay()
    {
        display = WlDisplayPtr(wl_display_connect(nullptr));
        if (!display)
        {
            throw std::runtime_error("Failed to create Wayland Display!");
        }
    }

    bool WaylandDisplay::flush() noexcept
    {
        return wl_display_flush(display.get()) != -1;
    }

    bool WaylandDisplay::dispatch() noexcept
    {
        return wl_display_dispatch(display.get()) != -1;
    }

    bool WaylandDisplay::roundtrip() noexcept
    {
        return wl_display_roundtrip(display.get()) != -1;
    }

    void WaylandDisplay::dispatch_pending() noexcept
    {
        wl_display_dispatch_pending(display.get());
    }

} // namespace tobi_engine
