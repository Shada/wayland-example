#pragma once

#include "wayland_client.hpp"
#include "window.hpp"

#include <memory>
#include <unordered_map>

namespace tobi_engine 
{

    class WindowRegistry 
    {
    public:

        WindowRegistry();
        ~WindowRegistry() = default;

        WindowRegistry(const WindowRegistry&) = delete;
        WindowRegistry& operator=(const WindowRegistry&) = delete;

        auto create_window(const WindowProperties& properties) -> std::shared_ptr<Window>;

    private:

        std::unique_ptr<WaylandClient> client;

        std::unordered_map<uint64_t, std::shared_ptr<Window>> windows;

    };

} // namespace tobi_engine
