#include "wayland_window.hpp"

#include <cstddef>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

#include "shared_memory.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "wayland_client.hpp"


namespace tobi_engine
{


namespace 
{

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {

    auto window = (WaylandWindow*)data;
    xdg_surface_ack_configure(xdg_surface, serial);

    Logger::debug("xdg_surface_configure()");

    if(!window->is_configured()) {
        window->resize();
    }
    window->draw();
}

const struct xdg_surface_listener xdg_surface_listener = 
{
    xdg_surface_configure
};

static void toplevel_configure(void *data, struct xdg_toplevel *toplevel, int32_t new_width, int32_t new_height, struct wl_array* states) {

    auto window = (WaylandWindow*)data;
    Logger::debug("toplevel_configure()");

    // No change
    if(!new_height && !new_width) 
        return ;
    
    window->resize(new_width, new_height);
}

static void toplevel_close(void* data, struct xdg_toplevel *toplevel) {

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


void frame_new(void* data, struct wl_callback* callback, uint32_t callback_data) {

    auto window = (WaylandWindow*)data;
    wl_callback_destroy(callback);
    callback = wl_surface_frame(window->get_surface());
    wl_callback_add_listener(callback, &callback_listener, window);
    
    window->set_callback(callback);

    window->draw();
}
    
}


WaylandWindow::WaylandWindow()
    :   client(WaylandClient::get_instance()),
        width(200),
        height(100),
        shared_memory(std::make_shared<SharedMemory>(width * height *4))
{
    initialize();
}

void WaylandWindow::initialize()
{
    surface = wl_compositor_create_surface(client->get_compositor());

    callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &callback_listener, this);

    x_surface = xdg_wm_base_get_xdg_surface(client->get_shell(), surface);
    xdg_surface_add_listener(x_surface, &xdg_surface_listener, this);

    x_toplevel = xdg_surface_get_toplevel(x_surface);
    xdg_toplevel_add_listener(x_toplevel, &toplevel_listener, this);
    xdg_toplevel_set_title(x_toplevel, "My test window: Wayland");

    draw();
}

void WaylandWindow::update()
{
    client->update();
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
    if (buffer) 
        wl_buffer_destroy(buffer);

    wl_shm_pool* pool = wl_shm_create_pool(client->get_shm(), shared_memory->get_fd(), width * height * PIXEL_SIZE);
    if (!pool)
    {
        throw std::runtime_error("Failed to create Wayland SHM pool.");
    }
    buffer = wl_shm_pool_create_buffer(pool, 0, width, height, width * PIXEL_SIZE, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    if (!buffer)
    {
        throw std::runtime_error("Failed to create Wayland buffer.");
    }
}

void WaylandWindow::draw()
{   
    background_colour++;
    
    shared_memory->fill(background_colour);
    
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, width, height);
    wl_surface_commit(surface);
}

void WaylandWindow::destroy()
{
    if (buffer)
    {
        wl_buffer_destroy(buffer);
        buffer = nullptr;
    }
    if(x_toplevel)
    {
        xdg_toplevel_destroy(x_toplevel);
        x_toplevel = nullptr;
    }
    if(x_surface) 
    {
        xdg_surface_destroy(x_surface);
        x_surface = nullptr;
    }
    if(callback) 
    {
        wl_callback_destroy(callback);
        callback = nullptr;
    }
    if(surface) 
    {
        wl_surface_destroy(surface);
        surface = nullptr;
    }
}

} // namespace tobi_engine
