#pragma once

#include "wayland_types.hpp"
#include "utils/logger.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <format>


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
         * @brief Create and initialize the Wayland registry.
         * @param display Pointer to a connected wl_display; must not be null.
         * @throws std::runtime_error on failure to obtain or roundtrip the registry.
         */
        explicit WaylandRegistry(wl_display* display);
        WaylandRegistry(const WaylandRegistry&) = delete;
        WaylandRegistry& operator=(const WaylandRegistry&) = delete;
        WaylandRegistry(WaylandRegistry&&) = default;
        WaylandRegistry& operator=(WaylandRegistry&&) = default;
        ~WaylandRegistry() noexcept = default;

        /**
         * @brief Get the Wayland protocol interface pointer.
         * @return Pointer to the Wayland interface.
         */
        template <typename Protocol>
        Protocol* get_protocol() const noexcept
        {
            return std::get<WlUniquePtr<Protocol>>(global_protocols).get();
        }

    private:
        /**
         * @brief Unavailable default constructor.
         */
        WaylandRegistry() = delete;
        /**
         * @brief Private delegating constructor.
         * @param display The wl_display pointer for roundtrip operations.
         * @param registry Already-created wl_registry wrapped in unique_ptr.
         */
        WaylandRegistry(wl_display* display, WlRegistryPtr&& registry);
        /**
         * @brief Obtain a new wl_registry from a display.
         * @param display The wl_display pointer to query.
         * @return A managed WlRegistryPtr.
         * @throws std::runtime_error if display is null or registry creation fails.
         */
        static WlRegistryPtr initialize_registry(wl_display* display);

        /**
         * @brief Bind all core Wayland protocols needed by the engine.
         */
        void bind_core_protocols();

        /**
         * @brief C callback: called when a global is added to the registry.
         */
        static void handle_registry_global_add(void *data,
                                               wl_registry *registry,
                                               uint32_t name,
                                               const char *interface,
                                               uint32_t version) noexcept;
        /**
         * @brief C callback: called when a global is removed from the registry.
         */
        static void handle_registry_global_remove(void *data,
                                                  wl_registry *registry,
                                                  uint32_t name) noexcept;
        /**
         * @brief Instance handler for a new global announcement.
         * @param registry The registry pointer.
         * @param name Unique id of the global object.
         * @param interface The interface name as a string.
         * @param version Version number of the interface.
         */
        void on_registry_global_add(wl_registry *registry,
                                   uint32_t name,
                                   std::string interface,
                                   uint32_t version);
        /**
         * @brief Instance handler for removal of a global object.
         * @param registry The registry pointer.
         * @param name Unique id of the removed global.
         */
        void on_registry_global_remove(wl_registry *registry,
                                       uint32_t name);

        
        /**
         * @brief Bind a Wayland interface by its traits.
         * @tparam Protocol The protocol type to bind.
         * @param required_version The version of the protocol to bind.
         * @throws std::runtime_error if the interface is not registered.
         */
        template<typename Protocol>
        void bind(uint32_t required_version = WaylandInterfaceTraits<Protocol>::version)
        {
            // Look up by the protocol's advertised interface name
            auto it = available_global_interfaces.find(WaylandInterfaceTraits<Protocol>::interface_name);
            if (it == available_global_interfaces.end()) 
            {
                LOG_WARNING("Interface {} is not registered in the Wayland registry!", WaylandInterfaceTraits<Protocol>::interface_name);
                throw std::runtime_error(
                    std::format("Failed to bind Wayland interface: {}", WaylandInterfaceTraits<Protocol>::interface_name)
                );
            }

            if (required_version > it->second.version) 
            {
                LOG_WARNING("Requested version {} for interface {} is higher than available version {}. Using available version.",
                            required_version, WaylandInterfaceTraits<Protocol>::interface_name, it->second.version);
                required_version = it->second.version;
            }
            
            Protocol *proxy = static_cast<Protocol*>(
                wl_registry_bind(
                    registry.get(),
                    it->second.name,
                    WaylandInterfaceTraits<Protocol>::interface, 
                    required_version));

            // Record the bound interface name by global id
            registered_global_interfaces[it->second.name] = WaylandInterfaceTraits<Protocol>::interface_name;

            bind_global<Protocol>(proxy);
        }

        /**
         * @brief Bind a global protocol to the registry.
         * @tparam Protocol The protocol type to bind.
         * @param proxy The protocol proxy to bind.
         */
        template<typename Protocol>
        void bind_global(Protocol* proxy) noexcept
        {
            std::get<WlUniquePtr<Protocol>>(global_protocols).reset(proxy);
        }

        WlRegistryPtr registry;
        CoreProtocols global_protocols{};

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

        static inline constexpr wl_registry_listener registry_listener 
        {
            &WaylandRegistry::handle_registry_global_add,
            &WaylandRegistry::handle_registry_global_remove
        };
    };

} // namespace tobi_engine
