#include "wayland_window.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>
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

void surface_ready_callback(void* data, struct wl_callback* callback, uint32_t callback_data) 
{
    auto window = static_cast<WaylandWindow*>(data);
    
    window->set_callback(nullptr);
}

const struct wl_callback_listener surface_ready_callback_listener = 
{
    .done=surface_ready_callback
};
    
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

void WaylandWindow::update_decoration_mode(bool enable)
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
        wl_callback_add_listener(callback.get(), &surface_ready_callback_listener, this);

        x_surface.reset(xdg_wm_base_get_xdg_surface(client->get_shell(), window_surface.get()));
        xdg_surface_add_listener(x_surface.get(), &xdg_surface_listener, this);

        wl_surface_set_user_data(window_surface.get(), this);
        wl_surface_set_user_data(content_surface.get(), this);

        wl_surface_attach(window_surface.get(), window_surface_buffer->get_buffer(), 0, 0);
        wl_surface_attach(content_surface.get(), content_surface_buffer->get_buffer(), 0, 0);

    }
    else
    {
        content_surface_buffer = std::make_unique<SurfaceBuffer>(this->properties.width, this->properties.height);
        content_surface.reset(wl_compositor_create_surface(client->get_compositor()));
        window_surface_buffer = nullptr;
        window_surface = nullptr;

        set_callback(wl_surface_frame(content_surface.get()));
        wl_callback_add_listener(callback.get(), &surface_ready_callback_listener, this);

        x_surface.reset(xdg_wm_base_get_xdg_surface(client->get_shell(), content_surface.get()));
        xdg_surface_add_listener(x_surface.get(), &xdg_surface_listener, this);
        
        wl_surface_set_user_data(content_surface.get(), this);

        wl_surface_attach(content_surface.get(), content_surface_buffer->get_buffer(), 0, 0);
    }

    x_toplevel.reset(xdg_surface_get_toplevel(x_surface.get()));
    xdg_toplevel_set_title(x_toplevel.get(), title.c_str());
    xdg_toplevel_set_app_id(x_toplevel.get(), title.c_str());
    xdg_toplevel_add_listener(x_toplevel.get(), &toplevel_listener, this);
    draw();
}

void WaylandWindow::update()
{
    client->update();

    for (auto& action : pending_actions)
        action();
    
    pending_actions.clear();
}

void WaylandWindow::on_key(uint32_t key, uint32_t state)
{
    char buf[128];
    xkb_keysym_t sym = xkb_state_key_get_one_sym(client->get_state(), key);
    xkb_keysym_get_name(sym, buf, sizeof(buf));

    const char *action =
            state == WL_KEYBOARD_KEY_STATE_PRESSED ? "press" : "release";
    fprintf(stderr, "key %s: sym: %-12s (%d), ", action, buf, sym);
    xkb_state_key_get_utf8(client->get_state(), key,
                       buf, sizeof(buf));
    fprintf(stderr, "utf8: '%s'\n", buf);
    if(state == WL_KEYBOARD_KEY_STATE_RELEASED)
    {
        switch (sym) 
        {
            case XKB_KEY_Escape:
                is_closed = true;
                break;
            case XKB_KEY_d:
            case XKB_KEY_D:
                pending_actions.push_back([this]() { update_decoration_mode(true); } );
                break;
            case XKB_KEY_f:
            case XKB_KEY_F: 
                pending_actions.push_back([this]() { update_decoration_mode(false); } );
                break;
            case XKB_KEY_a:
            case XKB_KEY_A:
                xdg_toplevel_set_fullscreen(x_toplevel.get(), nullptr);
                break;
            case XKB_KEY_s:
            case XKB_KEY_S:
                xdg_toplevel_unset_fullscreen(x_toplevel.get());
                break;
        }
    }
}

void WaylandWindow::on_pointer_button(uint32_t button, uint32_t state)
{
    if(button == 272) // left button
        Logger::debug("left click!");
    if(button == 273) // right button
        Logger::debug("right click!");
}

void WaylandWindow::resize(uint32_t width, uint32_t height)
{
    Logger::debug("resize(" + std::to_string(width) + ", " + std::to_string(height) + ")");
    content_surface_buffer->resize(width, height);
    wl_surface_attach(content_surface.get(), content_surface_buffer->get_buffer(), 0, 0);
    if(is_decorated)
    {
        window_surface_buffer->resize(width + DECORATIONS_BORDER_SIZE * 2, height + DECORATIONS_BORDER_SIZE + DECORATIONS_TOPBAR_SIZE);
        wl_surface_attach(window_surface.get(), window_surface_buffer->get_buffer(), 0, 0);
    }
    draw();
}

void WaylandWindow::draw()
{   
    if(is_decorated)
    {
        window_surface_buffer->fill(background_colour);

        wl_surface_damage(window_surface.get(), 0, 0, window_surface_buffer->get_width(), window_surface_buffer->get_height());
        wl_surface_commit(window_surface.get());
    }

    content_surface_buffer->fill(0xFFFFFFFF);

    wl_surface_damage(content_surface.get(), 0, 0, content_surface_buffer->get_width(), content_surface_buffer->get_height());
    wl_surface_commit(content_surface.get());
}

} // namespace tobi_engine
