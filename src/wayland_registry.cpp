#include "wayland_registry.hpp"
#include "wayland_display.hpp"
#include <cstdint>
#include <wayland-client.h>
#include <string_view>
#include <utility>

namespace tobi_engine {

using GlobalAddCallback = std::function<void(wl_registry*, uint32_t, const char*, uint32_t)>;

WaylandRegistry::WaylandRegistry(WaylandDisplay& display)
{
    registry = WlRegistryPtr(wl_display_get_registry(display.get()));
    if (!registry)
        throw std::runtime_error("Failed to create Wayland Registry!");
    add_listener();
}

wl_registry* WaylandRegistry::get() const {
    return registry.get();
}

void WaylandRegistry::add_listener() {
    wl_registry_add_listener(registry.get(), &registry_listener, this);
}

void WaylandRegistry::register_global_add_callback(std::string_view interface, GlobalAddCallback cb) {
    global_add_callbacks[interface] = std::move(cb);
}

void WaylandRegistry::register_global_remove_callback(std::function<void(wl_registry*, uint32_t)> cb) {
    global_remove_callback = std::move(cb);
}

void WaylandRegistry::registry_global_add(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    auto self = static_cast<WaylandRegistry*>(data);
    self->on_registry_global_add(registry, name, interface, version);
}

void WaylandRegistry::registry_global_remove(void *data, wl_registry *registry, uint32_t name) {
    auto self = static_cast<WaylandRegistry*>(data);
    self->on_registry_global_remove(registry, name);
}

void WaylandRegistry::on_registry_global_add(wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    auto it = global_add_callbacks.find(interface);
    if (it != global_add_callbacks.end()) {
        it->second(registry, name, interface, version);
    }
}

void WaylandRegistry::on_registry_global_remove(wl_registry *registry, uint32_t name) {
    if (global_remove_callback) {
        global_remove_callback(registry, name);
    }
}

} // namespace tobi_engine
