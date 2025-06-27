#include "wayland_window.hpp"

#include <cstdint>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include "wayland-xdg-shell-client-protocol.h"

#include "wayland_client.hpp"
#include "wayland_deleters.hpp"
#include "wayland_surface_buffer.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "window_registry.hpp"


namespace tobi_engine
{

namespace 
{

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) 
{
    Logger::debug("xdg_surface_configure()");

    auto window = (WaylandWindow*)data;

    xdg_surface_ack_configure(xdg_surface, serial);

    if(!window->is_configured()) 
        return;
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

    Logger::debug("toplevel_configure_bounds");
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
    Logger::debug("toplevel_configure_bounds done");
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
    Logger::debug("Frame start");
    if(!window->get_surface())
        return;
    
    auto cb = wl_surface_frame(window->get_surface());
    wl_callback_add_listener(cb, &callback_listener, window);
    window->set_callback(cb);

    window->draw();
    Logger::debug("Frame end");
}
    
}

WaylandWindow::WaylandWindow(const WindowProperties &properties)
    :   Window(properties),
        client(WaylandClient::get_instance()),
        window_surface_buffer(nullptr),
        content_surface_buffer(nullptr),
        title(properties.title)
{
    initialize();
}

void WaylandWindow::toggle_decorations(bool enable)
{
    if(enable == is_decorated)
        return;

    is_decorated = enable;

    if (callback) callback.reset();
    if (x_toplevel) x_toplevel.reset();
    if (x_surface) x_surface.reset();
    if (content_subsurface) content_subsurface.reset();

    // Set user data to nullptr before destroying surfaces
    if (window_surface) wl_surface_set_user_data(window_surface.get(), nullptr);
    if (content_surface) wl_surface_set_user_data(content_surface.get(), nullptr);

    // Destroy surfaces and buffers
    window_surface_buffer.reset();
    content_surface_buffer.reset();
    window_surface.reset();
    content_surface.reset();

    client->flush();
    client->clear();

    // Re-initialize with the new decoration state
    initialize();
}

void WaylandWindow::initialize()
{
    if(is_decorated)
    {
        window_surface_buffer = std::make_unique<SurfaceBuffer>(this->properties.width + DECORATIONS_BORDER_SIZE * 2, this->properties.height + DECORATIONS_BORDER_SIZE + DECORATIONS_TOPBAR_SIZE);
        content_surface_buffer = std::make_unique<SurfaceBuffer>(this->properties.width, this->properties.height);

        window_surface.reset(wl_compositor_create_surface(client->get_compositor()));

        content_surface.reset(wl_compositor_create_surface(client->get_compositor()));
        content_subsurface.reset(wl_subcompositor_get_subsurface(client->get_subcompositor(), content_surface.get(), window_surface.get()));
        wl_subsurface_set_desync(content_subsurface.get());
        wl_subsurface_set_position(content_subsurface.get(), DECORATIONS_BORDER_SIZE, DECORATIONS_TOPBAR_SIZE);

        set_callback(wl_surface_frame(window_surface.get()));
        wl_callback_add_listener(callback.get(), &callback_listener, this);

        x_surface.reset(xdg_wm_base_get_xdg_surface(client->get_shell(), window_surface.get()));
        xdg_surface_add_listener(x_surface.get(), &xdg_surface_listener, this);

        wl_surface_set_user_data(window_surface.get(), this);
        wl_surface_set_user_data(content_surface.get(), this);

        wl_surface_attach(window_surface.get(), window_surface_buffer->get_buffer(), 0, 0);
        wl_surface_attach(content_surface.get(), content_surface_buffer->get_buffer(), 0, 0);

        Logger::debug("Initialize Decorated window");

    }
    else
    {
        content_surface_buffer = std::make_unique<SurfaceBuffer>(this->properties.width, this->properties.height);
        content_surface.reset(wl_compositor_create_surface(client->get_compositor()));
        window_surface_buffer = nullptr;
        window_surface = nullptr;

        set_callback(wl_surface_frame(content_surface.get()));
        wl_callback_add_listener(callback.get(), &callback_listener, this);

        x_surface.reset(xdg_wm_base_get_xdg_surface(client->get_shell(), content_surface.get()));
        xdg_surface_add_listener(x_surface.get(), &xdg_surface_listener, this);
        
        wl_surface_set_user_data(content_surface.get(), this);

        wl_surface_attach(content_surface.get(), content_surface_buffer->get_buffer(), 0, 0);
        Logger::debug("Initialize Undecorated window");
    }

    x_toplevel.reset(xdg_surface_get_toplevel(x_surface.get()));
    xdg_toplevel_set_title(x_toplevel.get(), title.c_str());
    xdg_toplevel_set_app_id(x_toplevel.get(), title.c_str());
    xdg_toplevel_add_listener(x_toplevel.get(), &toplevel_listener, this);
    Logger::debug("Initialized window");
    draw();
}

void WaylandWindow::update()
{
    client->update();

    for (auto& action : pending_actions)
        action();
    
    pending_actions.clear();
}

void WaylandWindow::on_keypress(uint32_t key)
{
    if(key == 1)
        is_closed = true;
    if(key == 32 && !is_decorated)
        pending_actions.push_back([this]() { toggle_decorations(true); } );
    if(key == 33 && is_decorated)
        pending_actions.push_back([this]() { toggle_decorations(false); } );

    printf("on_keypress() == %d\n", key);
}

void WaylandWindow::resize(uint32_t width, uint32_t height)
{
    content_surface_buffer->resize(width, height);
    if(is_decorated)
        window_surface_buffer->resize(width + DECORATIONS_BORDER_SIZE * 2, height + DECORATIONS_BORDER_SIZE + DECORATIONS_TOPBAR_SIZE);
}

void WaylandWindow::draw()
{   
    if(is_decorated)
    {
        Logger::debug("Drawing Decorations");
        window_surface_buffer->fill(background_colour);

        wl_surface_damage(window_surface.get(), 0, 0, window_surface_buffer->get_width(), window_surface_buffer->get_height());
        wl_surface_commit(window_surface.get());

        Logger::debug("Drawing Decorations Done");
    }

    Logger::debug("Drawing Contents");
    content_surface_buffer->fill(0xFFFFFFFF);

    wl_surface_damage(content_surface.get(), 0, 0, content_surface_buffer->get_width(), content_surface_buffer->get_height());
    wl_surface_commit(content_surface.get());
    Logger::debug("Drawing Contents Done");
}

} // namespace tobi_engine
