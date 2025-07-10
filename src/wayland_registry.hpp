#pragma once

#include <functional>
#include <unordered_map>
#include <string_view>
#include <cstdint>

#include "wayland_types.hpp"

namespace tobi_engine 
{

    class WaylandDisplay;


    class WaylandRegistry 
    {
    public:
        using GlobalAddCallback = std::function<void(wl_registry*, uint32_t, const char*, uint32_t)>;
        WaylandRegistry(WaylandDisplay& display);

        wl_registry* get() const;
        void add_listener();
        void register_global_add_callback(std::string_view interface, GlobalAddCallback cb);
        void register_global_remove_callback(std::function<void(wl_registry*, uint32_t)> cb);
        // Called by Wayland
        static void registry_global_add(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
        static void registry_global_remove(void *data, wl_registry *registry, uint32_t name);
        void on_registry_global_add(wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
        void on_registry_global_remove(wl_registry *registry, uint32_t name);

    private:
        WlRegistryPtr registry;
        std::unordered_map<std::string_view, GlobalAddCallback> global_add_callbacks;
        std::function<void(wl_registry*, uint32_t)> global_remove_callback;
        static inline const wl_registry_listener registry_listener 
        {
            &WaylandRegistry::registry_global_add,
            &WaylandRegistry::registry_global_remove
        };
    };

} // namespace tobi_engine
