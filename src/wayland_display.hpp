#pragma once

#include "wayland_types.hpp"

namespace tobi_engine
{
    /**
     * @brief Wayland display connection (internal).
     */
    class WaylandDisplay
    {
    public:
        /**
         * @brief Connect to the Wayland server.
         * @throws std::runtime_error if the connection fails.
         */
        WaylandDisplay();

        WaylandDisplay(const WaylandDisplay&) = delete;
        WaylandDisplay& operator=(const WaylandDisplay&) = delete;
        WaylandDisplay(WaylandDisplay&&) = default;
        WaylandDisplay& operator=(WaylandDisplay&&) = default;
        ~WaylandDisplay() noexcept = default;

        /**
         * @brief Get the raw Wayland display pointer.
         * @return Pointer to the Wayland display.
         * @warning Do not call wl_display_disconnect on the returned pointer; it is managed by this class.
         */
        constexpr wl_display* get() const noexcept { return display.get(); }

        /**
         * @brief Flush pending requests.
         * @return True if successful, false otherwise.
         */
        [[nodiscard]] bool flush() noexcept;

        /**
         * @brief Dispatch events.
         * @return True if successful, false otherwise.
         */
        [[nodiscard]] bool dispatch() noexcept;

        /**
         * @brief Block until all requests are processed.
         * @return True if successful, false otherwise.
         */
        [[nodiscard]] bool roundtrip() noexcept;

        /**
         * @brief Dispatch pending events.
         */
        void dispatch_pending() noexcept;

    private:
        WlDisplayPtr display;
    };

} // namespace tobi_engine
