#pragma once

#include <cstdint>
#include <memory>

#include "window.hpp"

#include "wayland_deleters.hpp"

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
        void resize();
        
        void close_window() { is_closed = true; } 

        virtual bool should_close() override { return is_closed; }
        virtual void update() override;
        virtual void on_keypress(uint32_t key) override;

        bool is_configured(){ return configured; }

        void draw() ;

    private:
        void initialize();

        void create_buffer();

        uint16_t            width;
        uint16_t            height;
        const uint32_t      PIXEL_SIZE = 4;

        std::shared_ptr<WaylandClient> client;
        std::shared_ptr<SharedMemory> shared_memory;

        SurfacePtr          surface = nullptr;
        CallbackPtr         callback = nullptr;
        XdgSurfacePtr       x_surface = nullptr;
        XdgToplevelPtr      x_toplevel = nullptr;
        BufferPtr           buffer = nullptr;

        uint8_t             background_colour = 0;
        bool                is_closed = false;
        bool                configured = false;

        
};

}
