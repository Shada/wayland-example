#include "wayland_types.hpp"

#include <wayland-client-protocol.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>
#include "wayland-xdg-shell-client-protocol.h"
#include "wayland_window.hpp"

namespace tobi_engine
{

    void WlCursorThemeDeleter::operator()(wl_cursor_theme* ptr) const
    {
        if (ptr) wl_cursor_theme_destroy(ptr);
    }

    void KbStateDeleter::operator()(xkb_state* ptr) const 
    {
        if (ptr) xkb_state_unref(ptr);
    }
    void KbKeymapDeleter::operator()(xkb_keymap* ptr) const 
    {
        if (ptr) xkb_keymap_unref(ptr);
    }
    void KbContextDeleter::operator()(xkb_context* ptr) const 
    {
        if (ptr) xkb_context_unref(ptr);
    }
    void WlKeyboardDeleter::operator()(wl_keyboard* ptr) const 
    {
        if (ptr) wl_keyboard_destroy(ptr);
    }
    void WlPointerDeleter::operator()(wl_pointer* ptr) const 
    {
        if (ptr) wl_pointer_destroy(ptr);
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
        if(!ptr)
            return;
        
        auto t = static_cast<SurfaceUserData*>(wl_surface_get_user_data(ptr));
        if(t)
        {
            delete t;
            t = nullptr;
        }
        wl_surface_destroy(ptr);
        
    }
    void WlSubSurfaceDeleter::operator()(wl_subsurface* ptr) const 
    {
        if (ptr) wl_subsurface_destroy(ptr);
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
