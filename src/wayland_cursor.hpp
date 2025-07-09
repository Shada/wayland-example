#pragma once

#include "wayland_types.hpp"
#include <unordered_map>

namespace tobi_engine
{

    class WaylandClient;
    class WaylandCursor
    {
    public:
        explicit WaylandCursor(std::shared_ptr<WaylandClient> client);
        WaylandCursor(WaylandCursor &&) = default;
        WaylandCursor(const WaylandCursor &) = delete;
        WaylandCursor &operator=(WaylandCursor &&) = default;
        WaylandCursor &operator=(const WaylandCursor &) = delete;
        ~WaylandCursor() = default;

        void set_cursor(const std::string& cursor_name);
    
    private:
        SurfacePtr surface;
        CursorThemePtr theme;

        std::unordered_map<std::string_view, wl_cursor*> cursors;

        std::string current_cursor_name;
        std::string current_theme_name;
        uint32_t cursor_size;
        std::shared_ptr<WaylandClient> client;

        void draw();
    };
    

} 
