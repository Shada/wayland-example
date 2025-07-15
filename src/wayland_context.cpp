#include "wayland_context.hpp"

namespace tobi_engine
{

WaylandContext::WaylandContext()
{
    // Initialize the Wayland registry
    registry = std::make_unique<WaylandRegistry>(display.get());
    if (!display.roundtrip()) 
    {
        LOG_ERROR("Failed to roundtrip Wayland display during initialization!");
        throw std::runtime_error("Failed to initialize Wayland Client");
    }
    
    // Get the shared memory object from the registry
    compositor = registry->get_protocol<wl_compositor>();
    subcompositor = registry->get_protocol<wl_subcompositor>();
    shell = registry->get_protocol<xdg_wm_base>();
    shm = registry->get_protocol<wl_shm>();
    
    if (!shm || !compositor || !subcompositor || !shell) 
    {
        LOG_ERROR("Failed to bind required Wayland interfaces!");
        throw std::runtime_error("Failed to initialize Wayland Context");
    }
}

WaylandContext::~WaylandContext()
{
}

}
