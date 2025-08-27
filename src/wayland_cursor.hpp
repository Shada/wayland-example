#pragma once

#include "wayland_client.hpp"
#include "wayland_types.hpp"

#include <string_view>
#include <unordered_map>

namespace tobi_engine
{
    class WaylandCursor
    {
    public:

        WaylandCursor(WaylandClient* client);
        WaylandCursor(WaylandCursor &&) = default;
        WaylandCursor(const WaylandCursor &) = delete;
        WaylandCursor &operator=(WaylandCursor &&) = delete;
        WaylandCursor &operator=(const WaylandCursor &) = delete;
        ~WaylandCursor() = default;

        void set_cursor(const std::string& cursor_name);
    
    private:

        void draw();
        
        WlSurfacePtr surface;
        WlCursorThemePtr theme;
        WaylandClient *client;

        std::unordered_map<std::string_view, wl_cursor*> cursors;

        std::string current_cursor_name;
        std::string current_theme_name;
        uint32_t cursor_size;

    };

} // namespace tobi_engine
