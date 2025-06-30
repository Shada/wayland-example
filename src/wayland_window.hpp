#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "wayland_deleters.hpp"
#include "wayland_surface_buffer.hpp"
#include "window.hpp"

struct wl_subsurface;

namespace tobi_engine
{

class WaylandClient;

class WaylandWindow : public Window
{
    public:
        WaylandWindow(const WindowProperties &properties);
        virtual ~WaylandWindow() override {};
        
        wl_surface* get_surface() 
        {
            if(is_decorated && window_surface) 
                return window_surface.get();
            if(content_surface)
                return content_surface.get();
            return nullptr;
        }
        void set_callback(wl_callback *callback) { this->callback.reset(callback); }

        void resize(uint32_t width, uint32_t heigth);

        void close_window() { is_closed = true; } 

        virtual bool should_close() override { return is_closed; }
        virtual void update() override;
        virtual void on_keypress(uint32_t key) override;

        bool is_configured(){ return window_surface_buffer != nullptr; }

        void draw() ;

    private:
        virtual void initialize() override;
        void update_decoration_mode(bool enable);

        void create_buffer();

        std::shared_ptr<WaylandClient> client;
        std::unique_ptr<SurfaceBuffer> window_surface_buffer;
        std::unique_ptr<SurfaceBuffer> content_surface_buffer;

        std::vector<std::function<void()>> pending_actions;

        SurfacePtr          window_surface = nullptr;
        CallbackPtr         callback = nullptr;
        XdgSurfacePtr       x_surface = nullptr;
        XdgToplevelPtr      x_toplevel = nullptr;

        SurfacePtr          content_surface = nullptr;
        SubSurfacePtr       content_subsurface = nullptr;

        std::string         title;
        uint32_t            background_colour = 0xFF00DDDD;
        bool                is_closed = false;

        static constexpr uint32_t  DECORATIONS_BORDER_SIZE = 4;
        static constexpr uint32_t  DECORATIONS_TOPBAR_SIZE = 32;
        static constexpr uint32_t  DECORATIONS_BUTTON_SIZE = 28;

        bool is_decorated = true;
};

}
