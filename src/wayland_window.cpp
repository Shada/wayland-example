#include "wayland_window.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include "wayland_deleters.hpp"
#include "wayland-xdg-shell-client-protocol.h"

#include "shared_memory.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "wayland_client.hpp"


namespace tobi_engine
{

namespace 
{

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) 
{
    Logger::debug("xdg_surface_configure()");

    auto window = (WaylandWindow*)data;

    xdg_surface_ack_configure(xdg_surface, serial);

    if(!window->is_configured()) {
        window->resize();
    }
    window->draw();
}

const struct xdg_surface_listener xdg_surface_listener = 
{
    xdg_surface_configure
};

static void toplevel_configure(void *data, struct xdg_toplevel *toplevel, int32_t new_width, int32_t new_height, struct wl_array* states) 
{
    Logger::debug("toplevel_configure()");

    auto window = (WaylandWindow*)data;

    // No change
    if(!new_height && !new_width) 
        return;
    
    window->resize(new_width, new_height);
}

static void toplevel_close(void* data, struct xdg_toplevel *toplevel) 
{
    Logger::debug("toplevel_close()");
    auto window = (WaylandWindow*)data;
    window->close_window();
}

static void toplevel_configure_bounds(void *data, struct xdg_toplevel *toplevel, int32_t width, int32_t height) 
{ 
    auto window = static_cast<WaylandWindow*>(data);

    if (width > 0 && height > 0) 
    {
        Logger::debug("toplevel_configure_bounds: recommended bounds = " + std::to_string(width) + "x" + std::to_string(height));
        // Optionally, store these bounds in your window class for later use
        // window->set_recommended_bounds(width, height);
    } else 
    {
        Logger::debug("toplevel_configure_bounds: bounds unknown or not set");
        // Optionally, clear any stored bounds
        // window->clear_recommended_bounds();
    }
}

static void toplevel_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities)
{
    /* stub */ 
}

const struct xdg_toplevel_listener toplevel_listener = 
{
    toplevel_configure,
    toplevel_close,
    toplevel_configure_bounds,
    toplevel_wm_capabilities
};

void frame_new(void* data, struct wl_callback* callback, uint32_t callback_data);

const struct wl_callback_listener callback_listener = 
{
    frame_new
};


void frame_new(void* data, struct wl_callback* callback, uint32_t callback_data) 
{
    auto window = (WaylandWindow*)data;
    
    auto cb = wl_surface_frame(window->get_surface());
    wl_callback_add_listener(cb, &callback_listener, window);
    window->set_callback(cb);

    window->draw();
}
    
}

WaylandWindow::WaylandWindow(WindowProperties properties)
    :   client(WaylandClient::get_instance()),
        width(properties.width),
        height(properties.height),
        shared_memory(std::make_shared<SharedMemory>(width * height * PIXEL_SIZE))
{
    initialize();
}

void WaylandWindow::initialize()
{
    surface.reset(wl_compositor_create_surface(client->get_compositor()));

    set_callback(wl_surface_frame(surface.get()));
    wl_callback_add_listener(callback.get(), &callback_listener, this);

    x_surface.reset(xdg_wm_base_get_xdg_surface(client->get_shell(), surface.get()));
    xdg_surface_add_listener(x_surface.get(), &xdg_surface_listener, this);

    x_toplevel.reset(xdg_surface_get_toplevel(x_surface.get()));
    xdg_toplevel_set_title(x_toplevel.get(), "My test window");
    xdg_toplevel_set_app_id(x_toplevel.get(), "My test window");
    xdg_toplevel_add_listener(x_toplevel.get(), &toplevel_listener, this);
    
    draw();
}

void WaylandWindow::update()
{
    client->update();
}

void WaylandWindow::on_keypress(uint32_t key)
{
    if(key == 1)
        is_closed = true;

    printf("on_keypress() == %d\n", key);
}

void WaylandWindow::resize(uint16_t width, uint16_t height)
{
    if (this->width == width && this->height == height) 
        return;

    this->width = width;
    this->height = height;

    resize();
}
void WaylandWindow::resize()
{
    Logger::debug("resize()");

    configured = true;

    shared_memory->resize(width * height * PIXEL_SIZE);

    create_buffer();
}

void WaylandWindow::create_buffer()
{
    wl_shm_pool* pool = wl_shm_create_pool(client->get_shm(), shared_memory->get_fd(), width * height * PIXEL_SIZE);
    if (!pool)
        throw std::runtime_error("Failed to create Wayland SHM pool.");

    buffer.reset(wl_shm_pool_create_buffer(pool, 0, width, height, width * PIXEL_SIZE, WL_SHM_FORMAT_ARGB8888));
    wl_shm_pool_destroy(pool);
    if (!buffer)
        throw std::runtime_error("Failed to create Wayland buffer.");
}

void WaylandWindow::draw()
{   
    background_colour++;
    
    shared_memory->fill(background_colour);
    
    wl_surface_attach(surface.get(), buffer.get(), 0, 0);
    wl_surface_damage(surface.get(), 0, 0, width, height);
    wl_surface_commit(surface.get());
}

} // namespace tobi_engine
