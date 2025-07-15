#include "wayland_registry.hpp"

#include "utils/logger.hpp"
#include "wayland_types.hpp"


namespace tobi_engine 
{

WaylandRegistry::WaylandRegistry(wl_display* display)
    :   WaylandRegistry(
            display,
            initialize_registry(display)
        )
{
}

WaylandRegistry::WaylandRegistry(wl_display* display, WlRegistryPtr&& reg)
    :   registry(std::move(reg))
{
    
    if (!registry) 
    {
        LOG_ERROR("Wayland registry is null!");
        throw std::runtime_error("Failed to initialize Wayland Registry: registry is null");
    }
    
    wl_registry_add_listener(registry.get(), &registry_listener, this);

    // Roundtrip to ensure we receive all global interfaces
    if (!wl_display_roundtrip(display)) 
    {
        LOG_ERROR("Failed to roundtrip Wayland display during registry initialization!");
        throw std::runtime_error("Failed to initialize Wayland Registry");
    }
    bind_core_protocols();
}

WlRegistryPtr WaylandRegistry::initialize_registry(wl_display* display)
{
    if (!display) 
    {
        LOG_ERROR("Wayland display is null!");
        throw std::runtime_error("Failed to get Wayland registry: display is null");
    }

    WlRegistryPtr registry(wl_display_get_registry(display));
    if (!registry) 
    {
        LOG_ERROR("Failed to get Wayland registry from display!");
        throw std::runtime_error("Failed to get Wayland registry");
    }

    LOG_DEBUG("Created Wayland registry");
    
    return registry;
}

void WaylandRegistry::bind_core_protocols()
{
    bind<wl_compositor>();
    bind<wl_subcompositor>();
    bind<xdg_wm_base>();
    bind<wl_shm>();
    bind<wl_seat>();
}

void WaylandRegistry::handle_registry_global_add(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) noexcept
{
    auto self = static_cast<WaylandRegistry*>(data);
    self->on_registry_global_add(registry, name, interface, version);
}

void WaylandRegistry::handle_registry_global_remove(void *data, wl_registry *registry, uint32_t name) noexcept
{
    auto self = static_cast<WaylandRegistry*>(data);
    self->on_registry_global_remove(registry, name);
}

void WaylandRegistry::on_registry_global_add(wl_registry *registry, uint32_t name, std::string interface, uint32_t version)
{
    available_global_interfaces.insert( { interface, {name, version} } );
}

void WaylandRegistry::on_registry_global_remove(wl_registry *registry, uint32_t name) 
{
    if (registered_global_interfaces.contains(name)) 
    {
        LOG_DEBUG("Removing global interface: {}", registered_global_interfaces[name]);
        registered_global_interfaces.erase(name);
    }
    else 
    {
        LOG_WARNING("No registered interface found for name: {}", name);
    }
}

} // namespace tobi_engine
