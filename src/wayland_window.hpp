#pragma once

#include <cstdint>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include "xdg-shell-client-protocol.h"
#include "window.hpp"


static uint8_t                 colour = 0;
static bool                    is_closed = false; 

namespace tobi_engine
{

class MyWindow : public Window
{
    public:
        MyWindow(){ init(); }
        virtual ~MyWindow() override { destroy(); }
        
        wl_surface* get_surface() { return surface; }
        void set_callback(wl_callback *cb) { callback = cb; }

        void resize(uint16_t w, uint16_t h);
        void resize();
        bool should_close() { return is_closed; }

        virtual void update() override;

        bool is_configured(){ return (pixels != nullptr); }

        void draw() ;

    private:
        void init();
        void destroy();

        wl_surface          *surface;
        wl_callback         *callback;
        xdg_surface         *x_surface;
        xdg_toplevel        *x_top;
        wl_buffer           *buffer;

        uint8_t             *pixels = nullptr;
        uint16_t            width = 200;
        uint16_t            height = 100;
};

}
