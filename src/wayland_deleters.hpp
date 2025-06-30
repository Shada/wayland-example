#pragma once

#include <memory>
#include <wayland-client-protocol.h>

struct wl_keyboard;
struct wl_pointer;
struct wl_seat;
struct wl_shm;
struct xdg_wm_base;
struct wl_subcompositor;
struct wl_compositor;
struct wl_registry;
struct wl_display;

struct wl_surface;
struct wl_subsurface;
struct wl_callback;
struct xdg_surface;
struct xdg_toplevel;
struct wl_buffer;

namespace tobi_engine
{
    
    struct WlKeyboardDeleter { void operator()(wl_keyboard* ptr) const; };
    struct WlPointerDeleter { void operator()(wl_pointer* ptr) const; };
    struct WlSeatDeleter { void operator()(wl_seat* ptr) const; };
    struct WlShmDeleter { void operator()(wl_shm* ptr) const; };
    struct XdgShellDeleter { void operator()(xdg_wm_base* ptr) const; };
    struct WlSubCompositorDeleter { void operator()(wl_subcompositor* ptr) const; };
    struct WlCompositorDeleter { void operator()(wl_compositor* ptr) const; };
    struct WlRegistryDeleter { void operator()(wl_registry* ptr) const; };
    struct WlDisplayDeleter { void operator()(wl_display* ptr) const; };

    struct WlSurfaceDeleter { void operator()(wl_surface* ptr) const; };
    struct WlSubSurfaceDeleter { void operator()(wl_subsurface* ptr) const; };
    struct WlCallbackDeleter { void operator()(wl_callback* ptr) const; };
    struct WlBufferDeleter { void operator()(wl_buffer* ptr) const; };
    struct XdgSurfaceDeleter { void operator()(xdg_surface* ptr) const; };
    struct XdgToplevelDeleter { void operator()(xdg_toplevel* ptr) const; };

    using KeyboardPtr = std::unique_ptr<wl_keyboard, WlKeyboardDeleter>;
    using PointerPtr = std::unique_ptr<wl_pointer, WlPointerDeleter>;
    using SeatPtr = std::unique_ptr<wl_seat, WlSeatDeleter>;
    using ShmPtr = std::unique_ptr<wl_shm, WlShmDeleter>;
    using XdgShellPtr = std::unique_ptr<xdg_wm_base, XdgShellDeleter>;
    using SubCompositorPtr = std::unique_ptr<wl_subcompositor, WlSubCompositorDeleter>;
    using CompositorPtr = std::unique_ptr<wl_compositor, WlCompositorDeleter>;
    using RegistryPtr = std::unique_ptr<wl_registry, WlRegistryDeleter>;
    using DisplayPtr = std::unique_ptr<wl_display, WlDisplayDeleter>;

    using SurfacePtr = std::unique_ptr<wl_surface, WlSurfaceDeleter>;
    using SubSurfacePtr = std::unique_ptr<wl_subsurface, WlSubSurfaceDeleter>;
    using CallbackPtr = std::unique_ptr<wl_callback, WlCallbackDeleter>;
    using BufferPtr = std::unique_ptr<wl_buffer, WlBufferDeleter>;
    using XdgSurfacePtr = std::unique_ptr<xdg_surface, XdgSurfaceDeleter>;
    using XdgToplevelPtr = std::unique_ptr<xdg_toplevel, XdgToplevelDeleter>;
}
