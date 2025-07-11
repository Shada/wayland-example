#pragma once

#include "wayland_types.hpp"


#include <string>
#include <unordered_map>
#include <string_view>
#include <cstdint>
#include <wayland-client-protocol.h>

// TODO: I can do the roundtrip, which will add all available interfaces,
// Then I can have the bind_interface function that will bind the interface to the registry
// and return the pointer to the interface if it's available.

namespace tobi_engine 
{
    /**
        * @brief Data structure to hold information about available Wayland interfaces.
        * Contains the name and version of the interface.
        */
    struct WaylandInterfaceData
    {
        uint32_t name;
        uint32_t version;
    };
    class WaylandDisplay;

    /**
     * @class WaylandRegistry
     * @brief Manages the Wayland global registry and interface binding.
     * 
     * This class handles the Wayland registry, tracks available and registered interfaces,
     * and provides callback registration for global add/remove events. It is responsible
     * for adding listeners to the registry and facilitating interface binding.
     */
    class WaylandRegistry 
    {
    public:

        /**
         * @brief Constructs a WaylandRegistry for the given display.
         * @param display Reference to the WaylandDisplay.
         */
        explicit WaylandRegistry(WaylandDisplay& display);
        WaylandRegistry(const WaylandRegistry&) = delete;
        WaylandRegistry& operator=(const WaylandRegistry&) = delete;
        WaylandRegistry(WaylandRegistry&&) = default;
        WaylandRegistry& operator=(WaylandRegistry&&) = default;
        ~WaylandRegistry() noexcept = default;

        /**
         * @brief Bind a global interface by name.
         * @param interface_name The name of the global interface to bind.
         * @param interface The Wayland interface description.
         * @param version Desired version to bind.
         * @return Pointer to the bound interface, or nullptr on failure.
         */
        [[nodiscard]] void* bind_interface(std::string interface_name, const wl_interface* interface, uint32_t version) noexcept;
        
    private:
        void initialize();
        static void handle_registry_global_add(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version) noexcept;
        static void handle_registry_global_remove(void *data, wl_registry *registry, uint32_t name) noexcept;
        /**
         * @brief Instance handler for global add events.
         * @param registry The Wayland registry pointer.
         * @param name The unique name of the global object.
         * @param interface The interface name.
         * @param version The version of the global interface.
         */
        void on_registry_global_add(wl_registry *registry, uint32_t name, std::string  interface, uint32_t version);
        /**
         * @brief Instance handler for global remove events.
         * @param registry The Wayland registry pointer.
         * @param name The unique name of the global object.
         */
        void on_registry_global_remove(wl_registry *registry, uint32_t name);

        WlRegistryPtr registry;

        /**
         * @brief Tracks interfaces that have been registered and are currently available.
         * Used to avoid re-registering the same interface multiple times.
         */
        std::unordered_map<uint32_t, std::string> registered_global_interfaces;


        /**
         * @brief Tracks available interfaces that can be bound to.
         * Used for supporting manual interface binding.
         */
        std::unordered_map<std::string, WaylandInterfaceData> available_global_interfaces;

        static inline const wl_registry_listener registry_listener 
        {
            &WaylandRegistry::handle_registry_global_add,
            &WaylandRegistry::handle_registry_global_remove
        };
    };

} // namespace tobi_engine
