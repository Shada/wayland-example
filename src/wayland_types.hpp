#pragma once

#include <memory>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>
#include <wayland-xdg-shell-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

namespace tobi_engine
{

    struct WlBufferDeleter { void operator()(wl_buffer* ptr) const noexcept { if (ptr) wl_buffer_destroy(ptr); }; };
    using  WlBufferPtr = std::unique_ptr<wl_buffer, WlBufferDeleter>;
    struct WlCallbackDeleter { void operator()(wl_callback* ptr) const noexcept { if (ptr) wl_callback_destroy(ptr); }; };
    using  WlCallbackPtr = std::unique_ptr<wl_callback, WlCallbackDeleter>;
    struct WlCompositorDeleter { void operator()(wl_compositor* ptr) const  noexcept { if (ptr) wl_compositor_destroy(ptr); } };
    using  WlCompositorPtr = std::unique_ptr<wl_compositor, WlCompositorDeleter>;
    struct WlCursorThemeDeleter { void operator()(wl_cursor_theme* ptr) const noexcept { if (ptr) wl_cursor_theme_destroy(ptr); } };
    using  WlCursorThemePtr = std::unique_ptr<wl_cursor_theme, WlCursorThemeDeleter>;
    struct WlDisplayDeleter { void operator()(wl_display* ptr) const noexcept { if (ptr) { wl_display_flush(ptr); wl_display_disconnect(ptr); } } };
    using  WlDisplayPtr = std::unique_ptr<wl_display, WlDisplayDeleter>;
    struct WlKeyboardDeleter { void operator()(wl_keyboard* ptr) const noexcept { if (ptr) wl_keyboard_destroy(ptr); } };
    using  WlKeyboardPtr = std::unique_ptr<wl_keyboard, WlKeyboardDeleter>;
    struct WlPointerDeleter { void operator()(wl_pointer* ptr) const noexcept { if (ptr) wl_pointer_destroy(ptr); } };
    using  WlPointerPtr = std::unique_ptr<wl_pointer, WlPointerDeleter>;
    struct WlRegistryDeleter { void operator()(wl_registry* ptr) const noexcept { if (ptr) wl_registry_destroy(ptr); } };
    using  WlRegistryPtr = std::unique_ptr<wl_registry, WlRegistryDeleter>;
    struct WlSeatDeleter { void operator()(wl_seat* ptr) const noexcept { if (ptr) wl_seat_destroy(ptr); } };
    using  WlSeatPtr = std::unique_ptr<wl_seat, WlSeatDeleter>;
    struct WlShmDeleter { void operator()(wl_shm* ptr) const noexcept { if (ptr) wl_shm_destroy(ptr); } };
    using  WlShmPtr = std::unique_ptr<wl_shm, WlShmDeleter>;
    struct WlSubCompositorDeleter { void operator()(wl_subcompositor* ptr) const noexcept { if (ptr) wl_subcompositor_destroy(ptr); } };
    using  WlSubCompositorPtr = std::unique_ptr<wl_subcompositor, WlSubCompositorDeleter>;
    struct WlSubSurfaceDeleter { void operator()(wl_subsurface* ptr) const noexcept { if (ptr) wl_subsurface_destroy(ptr); }; };
    using  WlSubSurfacePtr = std::unique_ptr<wl_subsurface, WlSubSurfaceDeleter>;
    struct WlSurfaceDeleter { void operator()(wl_surface* ptr) const noexcept { if (ptr) wl_surface_destroy(ptr); } };
    using  WlSurfacePtr = std::unique_ptr<wl_surface, WlSurfaceDeleter>;
    
    struct XdgShellDeleter { void operator()(xdg_wm_base* ptr) const noexcept { if (ptr) xdg_wm_base_destroy(ptr); } };
    using  XdgShellPtr = std::unique_ptr<xdg_wm_base, XdgShellDeleter>;
    struct XdgSurfaceDeleter { void operator()(xdg_surface* ptr) const noexcept { if (ptr) xdg_surface_destroy(ptr); }; };
    using  XdgSurfacePtr = std::unique_ptr<xdg_surface, XdgSurfaceDeleter>;
    struct XdgToplevelDeleter { void operator()(xdg_toplevel* ptr) const noexcept { if (ptr) xdg_toplevel_destroy(ptr); }; };
    using  XdgToplevelPtr = std::unique_ptr<xdg_toplevel, XdgToplevelDeleter>;
    
    struct XkbContextDeleter { void operator()(xkb_context* ptr) const noexcept { if (ptr) xkb_context_unref(ptr); } };
    using  XkbContextPtr = std::unique_ptr<xkb_context, XkbContextDeleter>;
    struct XkbKeymapDeleter { void operator()(xkb_keymap* ptr) const noexcept { if (ptr) xkb_keymap_unref(ptr); } };
    using  XkbKeymapPtr = std::unique_ptr<xkb_keymap, XkbKeymapDeleter>;
    struct XkbStateDeteter { void operator()(xkb_state* ptr) const noexcept { if (ptr) xkb_state_unref(ptr); } };
    using  XkbStatePtr = std::unique_ptr<xkb_state, XkbStateDeteter>;
 
}