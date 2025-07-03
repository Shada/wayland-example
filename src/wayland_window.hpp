#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "wayland_deleters.hpp"
#include "wayland_surface.hpp"
#include "wayland_surface_buffer.hpp"
#include "window.hpp"

namespace tobi_engine
{

class WaylandClient;

class WaylandWindow : public Window
{
    public:
        WaylandWindow(const WindowProperties &properties);
        virtual ~WaylandWindow() override {};
        
        void set_callback(wl_callback *callback) { this->callback.reset(callback); }

        void resize(uint32_t width, uint32_t heigth);

        void close_window() { is_closed = true; } 

        virtual bool should_close() override { return is_closed; }
        virtual void update() override;
        virtual void on_key(uint32_t key, uint32_t state) override;
        virtual void on_pointer_button(uint32_t button, uint32_t state) override;

        bool is_configured(){ return !surfaces.empty(); }

        void draw() ;

    private:
        using WaylandSurfacePtr = std::shared_ptr<WaylandSurface>;
        
        virtual void initialize() override;
        void update_decoration_mode(bool enable);

        void create_buffer();

        std::shared_ptr<WaylandClient> client;

        std::vector<WaylandSurfacePtr> surfaces;

        CallbackPtr         callback;
        XdgSurfacePtr       x_surface;
        XdgToplevelPtr      x_toplevel;

        std::vector<std::function<void()>> pending_actions;

        std::string         title;
        uint32_t            background_colour = 0xFF00DDDD;
        bool                is_closed = false;

        const uint32_t DECORATIONS_BORDER_SIZE = 4;
        const uint32_t DECORATIONS_TOPBAR_SIZE = 32;
        const uint32_t DECORATIONS_BUTTON_SIZE = 28;
        const uint32_t WINDOW_MINIMUM_SIZE = 10;

        bool is_decorated = true;
};

struct SurfaceUserData
{
    WaylandWindow* window;
    enum SurfaceType { Decoration, Content } surface_type;
};

}
