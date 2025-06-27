#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <wayland-client-protocol.h>

#include "window.hpp"

#include "wayland_deleters.hpp"

struct wl_subsurface;

namespace tobi_engine
{

class WaylandClient;
class SharedMemory;

class WaylandWindow : public Window
{
    public:
        WaylandWindow(WindowProperties properties);
        virtual ~WaylandWindow() override = default;
        
        wl_surface* get_surface() { return surface.get(); }
        void set_callback(wl_callback *callback) { this->callback.reset(callback); }

        void resize(uint16_t width, uint16_t heigth);

        void close_window() { is_closed = true; } 

        virtual bool should_close() override { return is_closed; }
        virtual void update() override;
        virtual void on_keypress(uint32_t key) override;

        bool is_configured(){ return shared_memory != nullptr; }

        void draw() ;

    private:
        void initialize();

        void create_buffer();

        std::shared_ptr<WaylandClient> client;
        std::shared_ptr<SharedMemory> shared_memory;

        SurfacePtr          surface = nullptr;
        CallbackPtr         callback = nullptr;
        XdgSurfacePtr       x_surface = nullptr;
        XdgToplevelPtr      x_toplevel = nullptr;

        std::string         title;
        uint8_t             background_colour = 0;
        bool                is_closed = false;
        
};

}
