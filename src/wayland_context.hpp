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

    std::unique_ptr<WaylandDisplay> display;
    std::unique_ptr<WaylandRegistry> registry;

};


}