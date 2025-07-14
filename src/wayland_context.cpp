#include "wayland_context.hpp"

namespace tobi_engine
{

WaylandContext::WaylandContext()
{
    // Initialize the Wayland registry
    registry = std::make_unique<WaylandRegistry>(display);
    
    // Get the shared memory object from the registry
    compositor = registry->bind<wl_compositor>();
    subcompositor = registry->bind<wl_subcompositor>();
    shell = registry->bind<xdg_wm_base>();
    shm = registry->bind<wl_shm>();
    
    if (!shm)
    {
        LOG_ERROR("Failed to bind wl_shm interface!");
        throw std::runtime_error("Failed to initialize Wayland Context");
    }
}

WaylandContext::~WaylandContext()
{
}

}
