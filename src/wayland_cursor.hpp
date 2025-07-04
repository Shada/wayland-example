#pragma once

#include "wayland_deleters.hpp"

namespace tobi_engine
{

    class WaylandClient;
    class WaylandCursor
    {
    public:
        WaylandCursor(const WaylandClient *client);
        WaylandCursor(WaylandCursor &&) = default;
        WaylandCursor(const WaylandCursor &) = delete;
        WaylandCursor &operator=(WaylandCursor &&) = default;
        WaylandCursor &operator=(const WaylandCursor &) = delete;
        ~WaylandCursor() = default;

        void set_cursor(const std::string& cursor_name);
    
    private:
        CursorThemePtr theme;
        SurfacePtr surface;
        BufferPtr buffer;

        std::string current_cursor_name;
        uint32_t cursor_size;
        void draw();
    };
    

} 
