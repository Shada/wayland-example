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

class MyWindow : public Window
{
    public:
        MyWindow();
        virtual ~MyWindow() override { destroy(); }
        
        wl_surface* get_surface() { return surface; }
        void set_callback(wl_callback *cb) { callback = cb; }

        void resize(uint16_t w, uint16_t h);
        void resize();
        bool should_close() { return is_closed; }
        void close_window() { is_closed = true; } 

        virtual void update() override;

        bool is_configured(){ return (pixels != nullptr); }

        void draw() ;

    private:
        void initialize();
        void destroy();

        void create_buffer();
        void create_shared_memory();

        std::shared_ptr<WaylandClient> client;
        std::shared_ptr<SharedMemory> shared_memory;

        wl_surface          *surface = nullptr;
        wl_callback         *callback = nullptr;
        xdg_surface         *x_surface = nullptr;
        xdg_toplevel        *x_toplevel = nullptr;
        wl_buffer           *buffer = nullptr;

        uint8_t             *pixels = nullptr;
        uint16_t            width = 200;
        uint16_t            height = 100;

        uint8_t                 colour = 0;
        bool                    is_closed = false; 

        
};

}
