#include "wayland_registry.hpp"

#include "utils/logger.hpp"
#include "wayland_display.hpp"

#include <cstdint>
#include <stdexcept>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

namespace tobi_engine 
{

WaylandRegistry::WaylandRegistry(WaylandDisplay& display)
    : registry(WlRegistryPtr(wl_display_get_registry(display.get())))
{
    initialize();

    if (!display.roundtrip())
    {
        LOG_ERROR("Failed to roundtrip Wayland display during registry initialization!");
        throw std::runtime_error("Failed to initialize Wayland Registry");
    }
}

void WaylandRegistry::initialize()
{
    if (!registry)
        throw std::runtime_error("Failed to create Wayland Registry!");

    wl_registry_add_listener(registry.get(), &registry_listener, this);
}

void* WaylandRegistry::bind_interface(std::string interface_name, const wl_interface* interface, uint32_t version) noexcept
{
    if (!registry)
    {
        LOG_ERROR("WaylandRegistry is not initialized!");
        return nullptr;
    }

    if (interface_name.empty())
    {
        LOG_ERROR("Interface name cannot be empty!");
        return nullptr;
    }

    auto it = available_global_interfaces.find(interface_name.data()); 

    if (it == available_global_interfaces.end())
    {
        LOG_ERROR("Interface {} is not registered in the Wayland registry!", interface_name);
        return nullptr;
    }
    
    if (version > it->second.version)
    {
        LOG_ERROR("Requested version {} for interface {} is not supported (available: 1-{})", 
            version, interface_name, it->second.version);
        return nullptr;
    }

    LOG_DEBUG("Binding interface {} with version {}", interface_name, version);

    registered_global_interfaces[it->second.name] = interface_name;
    return wl_registry_bind(registry.get(), it->second.name, interface, version);
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

void WaylandRegistry::on_registry_global_add(wl_registry *registry, uint32_t name, std::string  interface, uint32_t version)
{
    available_global_interfaces.insert( { interface.data(), {name, version} } );
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
