#include "wayland_window.hpp"

#include <cstdint>
#include <format>
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
#include "wayland_types.hpp"
#include "wayland_surface.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "window_registry.hpp"


namespace tobi_engine
{

namespace 
{

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) 
{
    LOG_DEBUG("xdg_surface_configure()");

    auto window = static_cast<WaylandWindow*>(data);

    xdg_surface_ack_configure(xdg_surface, serial);

    if(!window->is_configured()) 
        return;

    window->draw();
}

const struct xdg_surface_listener xdg_surface_listener = 
{
    xdg_surface_configure
};

// should remove when supporting resizing with decorations
static void toplevel_configure(void *data, struct xdg_toplevel *toplevel, int32_t new_width, int32_t new_height, struct wl_array* states) 
{
    LOG_DEBUG("toplevel_configure()");

    auto window = static_cast<WaylandWindow*>(data);

    // No change
    if(!new_height && !new_width) 
        return;
    
    window->resize(new_width, new_height);
}

static void toplevel_close(void* data, struct xdg_toplevel *toplevel) 
{
    auto window = static_cast<WaylandWindow*>(data);
    window->close_window();
}

static void toplevel_configure_bounds(void *data, struct xdg_toplevel *toplevel, int32_t width, int32_t height) 
{ 
    auto window = static_cast<WaylandWindow*>(data);

    LOG_DEBUG("toplevel_configure_bounds");
    if (width > 0 && height > 0) 
    {
        LOG_DEBUG("recommended bounds = {}x{}", width, height);
        // Optionally, store these bounds in your window class for later use
        // window->set_recommended_bounds(width, height);
    } else 
    {
        LOG_DEBUG("bounds unknown or not set");
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

PointerEvent WaylandWindow::pointer_event = {};

WaylandWindow::WaylandWindow(const WindowProperties &properties)
    :   Window(properties),
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

    // Destroy surfaces and buffers
    surfaces.clear();

    WaylandClient::get_instance().flush();
    WaylandClient::get_instance().clear();

    // Re-initialize with the new decoration state
    initialize();
}

void WaylandWindow::initialize()
{
    auto & client = WaylandClient::get_instance();
    cursor = std::make_unique<WaylandCursor>();
    auto shell = client.get_shell().value_or(nullptr);
    if (!shell)
    {
        LOG_ERROR("Failed to get shell");
        return;
    }
    if(is_decorated)
    {
        surfaces.push_back(std::make_unique<DecorationSurface>(this->properties.width, this->properties.height));
        surfaces.push_back(std::make_unique<ContentSurface>(this->properties.width, this->properties.height, surfaces[0].get()));

        set_callback(wl_surface_frame(surfaces[0]->get_surface()));
        wl_callback_add_listener(callback.get(), &surface_ready_callback_listener, this);


        x_surface.reset(xdg_wm_base_get_xdg_surface(shell, surfaces[0]->get_surface()));
        xdg_surface_add_listener(x_surface.get(), &xdg_surface_listener, this);

        wl_surface_set_user_data(surfaces[0]->get_surface(), this);
        wl_surface_set_user_data(surfaces.back()->get_surface(), this);
    }
    else
    {
        surfaces.push_back(std::make_unique<ContentSurface>(this->properties.width, this->properties.height));

        set_callback(wl_surface_frame(surfaces.back()->get_surface()));
        wl_callback_add_listener(callback.get(), &surface_ready_callback_listener, this);

        x_surface.reset(xdg_wm_base_get_xdg_surface(shell, surfaces.back()->get_surface()));
        xdg_surface_add_listener(x_surface.get(), &xdg_surface_listener, this);

        wl_surface_set_user_data(surfaces.back()->get_surface(), this);
    }

    x_toplevel.reset(xdg_surface_get_toplevel(x_surface.get()));
    xdg_toplevel_set_title(x_toplevel.get(), title.c_str());
    xdg_toplevel_set_min_size(x_toplevel.get(), 
        WINDOW_MINIMUM_SIZE + DECORATIONS_TOPBAR_SIZE + DECORATIONS_BORDER_SIZE, 
        WINDOW_MINIMUM_SIZE + DECORATIONS_TOPBAR_SIZE + DECORATIONS_BORDER_SIZE);
    xdg_toplevel_set_app_id(x_toplevel.get(), title.c_str());
    xdg_toplevel_add_listener(x_toplevel.get(), &toplevel_listener, this);
    
    draw();
}

void WaylandWindow::update()
{
    WaylandClient::get_instance().update();

    for (auto& action : pending_actions)
        action();
    
    pending_actions.clear();
}

void WaylandWindow::on_key(uint32_t key, uint32_t state)
{
    auto & client = WaylandClient::get_instance();
    char buf[128];
    xkb_keysym_t sym = xkb_state_key_get_one_sym(client.get_state(), key);
    xkb_keysym_get_name(sym, buf, sizeof(buf));

    const char *action =
            state == WL_KEYBOARD_KEY_STATE_PRESSED ? "press" : "release";
    LOG_DEBUG("key {}: sym: {} ({})", action, buf, sym);

    xkb_state_key_get_utf8(client.get_state(), key,
                        buf, sizeof(buf));
    if(buf[0] > 32)
    {
        LOG_DEBUG("utf8: {}", (uint8_t(buf[0])));
    }

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
        LOG_DEBUG("left click!");
    if(button == 273) // right button
        LOG_DEBUG("right click! {}", button);
}
void WaylandWindow::on_pointer_motion(int32_t x, int32_t y)
{
    LOG_DEBUG("Pointer moved to: ({}, {})", x, y);
    
    pointer_position.x = x;
    pointer_position.y = y;
}

void WaylandWindow::resize(uint32_t width, uint32_t height)
{
    if(is_decorated)
    {
        const auto decoration_width =  DECORATIONS_BORDER_SIZE * 2;
        const auto decoration_height = DECORATIONS_BORDER_SIZE + DECORATIONS_TOPBAR_SIZE;

        this->properties.width = std::max(int32_t(width - decoration_width), (int32_t)WINDOW_MINIMUM_SIZE);
        this->properties.height = std::max(int32_t(height - decoration_height), (int32_t)WINDOW_MINIMUM_SIZE);
   }
    else
    {
        this->properties.width = std::max(width, WINDOW_MINIMUM_SIZE);
        this->properties.height = std::max(height, WINDOW_MINIMUM_SIZE);
    }

    for (auto &surface : surfaces)
    {
        surface->resize(this->properties.width, this->properties.height);
    }
}

void WaylandWindow::draw()
{   
    for (auto surface : surfaces)
        surface->draw();
}

} // namespace tobi_engine
