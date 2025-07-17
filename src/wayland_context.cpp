#include "wayland_context.hpp"

namespace tobi_engine
{

WaylandContext::WaylandContext()
    :   display(std::make_unique<WaylandDisplay>()),
        registry(std::make_unique<WaylandRegistry>(display->get()))
{
}

WaylandContext::~WaylandContext()
{
}

}
