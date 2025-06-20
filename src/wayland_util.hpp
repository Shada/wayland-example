#pragma once


#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

namespace tobi_engine
{

void registry_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
void registry_global_remove(void *data, struct wl_registry *registry, uint32_t name);

void frame_new(void* data, struct wl_callback* callback, uint32_t callback_data);

static const struct wl_callback_listener callback_listener = 
{
    .done = frame_new
};

void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial);

static const struct xdg_surface_listener xdg_surface_listener = 
{
    .configure = xdg_surface_configure
};

void toplevel_configure(void *data, struct xdg_toplevel *toplevel, int32_t new_width, int32_t new_height, struct wl_array* states);
void toplevel_close(void* data, struct xdg_toplevel *toplevel);

static const struct xdg_toplevel_listener toplevel_listener = 
{
    .configure = toplevel_configure,
    .close = toplevel_close
};

}
