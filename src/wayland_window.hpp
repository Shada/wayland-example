#pragma once

#include <cstdint>
#include <memory>

#include "window.hpp"

struct wl_surface;
struct wl_callback;
struct xdg_surface;
struct xdg_toplevel;
struct wl_buffer;

namespace tobi_engine
{

class WaylandClient;
class SharedMemory;

class WaylandWindow : public Window
{
    public:
        WaylandWindow();
        virtual ~WaylandWindow() override { destroy(); }
        
        wl_surface* get_surface() { return surface; }
        void set_callback(wl_callback *callback) { this->callback = callback; }

        void resize(uint16_t width, uint16_t heigth);
        void resize();
        bool should_close() { return is_closed; }
        void close_window() { is_closed = true; } 

        virtual void update() override;

        bool is_configured(){ return configured; }

        void draw() ;

    private:
        void initialize();
        void destroy();

        void create_buffer();

        uint16_t            width;
        uint16_t            height;
        const uint32_t      PIXEL_SIZE = 4;

        std::shared_ptr<WaylandClient> client;
        std::shared_ptr<SharedMemory> shared_memory;

        wl_surface          *surface = nullptr;
        wl_callback         *callback = nullptr;
        xdg_surface         *x_surface = nullptr;
        xdg_toplevel        *x_toplevel = nullptr;
        wl_buffer           *buffer = nullptr;

        uint8_t             background_colour = 0;
        bool                is_closed = false;
        bool                configured = false;

        
};

}
