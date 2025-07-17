#pragma once

#include <memory>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>
#include <wayland-xdg-shell-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

namespace tobi_engine
{

    // Wayland Interface Traits for binding and management
    template<typename T> struct WaylandInterfaceTraits;
    template<> struct WaylandInterfaceTraits<wl_compositor> 
    { 
        // The protocol name as advertised by Wayland
        static constexpr const char* interface_name = "wl_compositor";
        static constexpr const wl_interface* interface = &wl_compositor_interface;
        static constexpr uint32_t version = 6;
    };
    template<> struct WaylandInterfaceTraits<wl_subcompositor> 
    { 
        static constexpr const char* interface_name = "wl_subcompositor";
        static constexpr const wl_interface* interface = &wl_subcompositor_interface;
        static constexpr uint32_t version = 1;
    };
    template<> struct WaylandInterfaceTraits<wl_shm>
    { 
        static constexpr const char* interface_name = "wl_shm";
        static constexpr const wl_interface* interface = &wl_shm_interface;
        static constexpr uint32_t version = 2;
    };
    template<> struct WaylandInterfaceTraits<xdg_wm_base>
    { 
        static constexpr const char* interface_name = "xdg_wm_base";
        static constexpr const wl_interface* interface = &xdg_wm_base_interface;
        static constexpr uint32_t version = 6;
    };
    template<> struct WaylandInterfaceTraits<wl_seat>
    { 
        static constexpr const char* interface_name = "wl_seat";
        static constexpr const wl_interface* interface = &wl_seat_interface;
        static constexpr uint32_t version = 4; // only supporting version 4 for now
    };

    // Templated unique pointer deleters for Wayland proxy objects

    template <typename T>
    struct WlDeleter
    {
        constexpr WlDeleter() noexcept = default;
        void operator()(T* ptr) const noexcept
        {
            if (ptr) 
                wl_proxy_destroy(reinterpret_cast<wl_proxy*>(ptr));
        }
    };
    
    template<typename T>
    using WlUniquePtr = std::unique_ptr<T, WlDeleter<T>>;

    using CoreProtocols =
        std::tuple<
            WlUniquePtr<wl_compositor>,
            WlUniquePtr<wl_subcompositor>,
            WlUniquePtr<wl_shm>,
            WlUniquePtr<xdg_wm_base>,
            WlUniquePtr<wl_seat>
        >;

    using WlCompositorPtr = WlUniquePtr<wl_compositor>;
    using WlSubCompositorPtr = WlUniquePtr<wl_subcompositor>;
    using WlShmPtr = WlUniquePtr<wl_shm>;

    // xdg_wm_base needs to be handled separately as it has a custom listener
    using XdgShellPtr = WlUniquePtr<xdg_wm_base>;
    using WlSeatPtr = WlUniquePtr<wl_seat>;
    
    struct WlBufferDeleter { void operator()(wl_buffer* ptr) const noexcept { if (ptr) wl_buffer_destroy(ptr); }; };
    using  WlBufferPtr = std::unique_ptr<wl_buffer, WlBufferDeleter>;
    struct WlCallbackDeleter { void operator()(wl_callback* ptr) const noexcept { if (ptr) wl_callback_destroy(ptr); }; };
    using  WlCallbackPtr = std::unique_ptr<wl_callback, WlCallbackDeleter>;
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
    struct WlSubSurfaceDeleter { void operator()(wl_subsurface* ptr) const noexcept { if (ptr) wl_subsurface_destroy(ptr); }; };
    using  WlSubSurfacePtr = std::unique_ptr<wl_subsurface, WlSubSurfaceDeleter>;
    struct WlSurfaceDeleter { void operator()(wl_surface* ptr) const noexcept { if (ptr) wl_surface_destroy(ptr); } };
    using  WlSurfacePtr = std::unique_ptr<wl_surface, WlSurfaceDeleter>;
    

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
