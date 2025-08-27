#pragma once

#include "wayland_client.hpp"
#include "wayland_cursor.hpp"
#include "wayland_types.hpp"
#include "wayland_surface.hpp"
#include "wayland_surface_buffer.hpp"
#include "window.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace tobi_engine
{

    struct Position
    {
        int32_t x = 0;
        int32_t y = 0;
    };

    struct PointerEvent
    {
        Position position;
        uint32_t button = 0;
        uint32_t state = 0; // 0 for released, 1 for pressed
        uint32_t serial = 0; // Serial number for the event
        wl_pointer *pointer = nullptr;
    };

    class WaylandWindow : public Window
    {
    public:

        WaylandWindow(const WindowProperties &properties, WaylandClient* client);
        virtual ~WaylandWindow() override = default;

        void set_callback(wl_callback *callback);

        void resize(uint32_t width, uint32_t heigth);

        void close_window();

        virtual bool should_close() override;
        virtual void update() override;
        virtual void on_key(uint32_t key, uint32_t state) override;
        virtual void on_pointer_button(uint32_t button, uint32_t state) override;

        virtual void on_pointer_motion(int32_t x, int32_t y) override;

        bool is_configured();

        void draw();

        void update_cursor(const std::string &cursor_name);

        // public static for now.
        static PointerEvent pointer_event;

    private:
        
        virtual void initialize() override;
        void update_decoration_mode(bool enable);

        void create_buffer();

        WaylandClient* client;

        std::unique_ptr<WaylandCursor> cursor;

        std::vector<std::shared_ptr<WaylandSurface>> surfaces;

        WlCallbackPtr callback;
        XdgSurfacePtr x_surface;
        XdgToplevelPtr x_toplevel;

        std::vector<std::function<void()>> pending_actions;

        bool is_closed = false;

        const uint32_t DECORATIONS_BORDER_SIZE = 4;
        const uint32_t DECORATIONS_TOPBAR_SIZE = 32;
        const uint32_t DECORATIONS_BUTTON_SIZE = 28;
        const uint32_t WINDOW_MINIMUM_SIZE = 10;

        Position pointer_position;

        bool is_decorated = true;
    
    };

} // namespace tobi_engine
