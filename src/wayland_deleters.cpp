#include "wayland_deleters.hpp"

#include <wayland-client-protocol.h>
#include "wayland-xdg-shell-client-protocol.h"

namespace tobi_engine
{
    void WlKeyboardDeleter::operator()(wl_keyboard* ptr) const 
    {
        if (ptr) wl_keyboard_destroy(ptr);
    }
    void WlSeatDeleter::operator()(wl_seat* ptr) const 
    {
        if (ptr) wl_seat_destroy(ptr);
    }
    void WlShmDeleter::operator()(wl_shm* ptr) const 
    {
        if (ptr) wl_shm_destroy(ptr);
    }
    void XdgShellDeleter::operator()(xdg_wm_base* ptr) const 
    {
        if (ptr) xdg_wm_base_destroy(ptr);
    }
    void WlSubCompositorDeleter::operator()(wl_subcompositor* ptr) const 
    {
        if (ptr) wl_subcompositor_destroy(ptr);
    }
    void WlCompositorDeleter::operator()(wl_compositor* ptr) const 
    {
        if (ptr) wl_compositor_destroy(ptr);
    }
    void WlRegistryDeleter::operator()(wl_registry* ptr) const 
    {
        if (ptr) wl_registry_destroy(ptr);
    }
    void WlDisplayDeleter::operator()(wl_display* ptr) const 
    {
        if (ptr) wl_display_disconnect(ptr);
    }

    void WlSurfaceDeleter::operator()(wl_surface* ptr) const 
    {
        if (ptr) wl_surface_destroy(ptr);
    }
    void WlCallbackDeleter::operator()(wl_callback* ptr) const 
    {
        if (ptr) wl_callback_destroy(ptr);
    }
    void WlBufferDeleter::operator()(wl_buffer* ptr) const 
    {
        if (ptr) wl_buffer_destroy(ptr);
    }
    void XdgSurfaceDeleter::operator()(xdg_surface* ptr) const 
    {
        if (ptr) xdg_surface_destroy(ptr);
    }
    void XdgToplevelDeleter::operator()(xdg_toplevel* ptr) const 
    {
        if (ptr) xdg_toplevel_destroy(ptr);
    }

}
