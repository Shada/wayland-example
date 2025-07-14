#pragma once


#include "utils/logger.hpp"
#include "wayland_types.hpp"
#include "wayland_display.hpp"
#include "wayland_registry.hpp"
#include <memory>
#include <stdexcept>

namespace tobi_engine
{

class WaylandContext
{
public:
    WaylandContext();
    WaylandContext(WaylandContext &&) = default;
    WaylandContext(const WaylandContext &) = default;
    WaylandContext &operator=(WaylandContext &&) = default;
    WaylandContext &operator=(const WaylandContext &) = default;
    ~WaylandContext();

private:
    // Private members can be added here
    WaylandDisplay display;
    std::unique_ptr<WaylandRegistry> registry;
    WlUniquePtr<wl_compositor> compositor;
    WlUniquePtr<wl_subcompositor> subcompositor;
    WlUniquePtr<xdg_wm_base> shell;
    WlUniquePtr<wl_shm> shm;
};


}