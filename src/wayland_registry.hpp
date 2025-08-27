#pragma once

#include "wayland_types.hpp"
#include "utils/logger.hpp"

#include <string>
#include <unordered_map>
#include <cstdint>
#include <variant>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

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


        wl_compositor* get_compositor() const noexcept
        {
            return get_interface<wl_compositor>();
        }
        wl_subcompositor* get_subcompositor() const noexcept
        {
            return get_interface<wl_subcompositor>();
        }
        xdg_wm_base* get_shell() const noexcept
        {
            return get_interface<xdg_wm_base>();
        }
        wl_shm* get_shm() const noexcept
        {
            return get_interface<wl_shm>();
        }
        wl_seat* get_seat() const noexcept
        {
            return get_interface<wl_seat>();
        }

    private:
    
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

        void bind_core_protocols();

        /**
         * @brief Get the Wayland protocol interface pointer.
         * @return Pointer to the Wayland interface.
         */
        template <typename T>
        T* get_interface() const noexcept
        {
            try 
            {
                return std::get<WlUniquePtr<T>>(global_protocols).get();
            } 
            catch (std::bad_variant_access const& exception)
            {
                LOG_ERROR("Wayland interface not supported: {}", exception.what());
                return nullptr;
            }
        }

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
        template<typename WaylandInterface>
        void register_interface(uint32_t required_version = WaylandInterfaceTraits<WaylandInterface>::version)
        {
            auto proxy = reinterpret_cast<WaylandInterface*>(bind_wayland_interface(WaylandInterfaceTraits<WaylandInterface>::interface_name,
                  WaylandInterfaceTraits<WaylandInterface>::interface,
                  required_version));

            std::get<WlUniquePtr<WaylandInterface>>(global_protocols).reset(proxy);
        }

        wl_proxy* bind_wayland_interface(const std::string& interface_name, const wl_interface* interface, uint32_t version);

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
