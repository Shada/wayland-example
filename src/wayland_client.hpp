#pragma once

#include "wayland_types.hpp"
#include "wayland_window.hpp"
#include "wayland_display.hpp"
#include "wayland_registry.hpp"

namespace tobi_engine
{

class WaylandClient
{
    public:

        static WaylandClient& get_instance()
        {
            static WaylandClient instance;
            return instance;
        }
        
        ~WaylandClient() = default;
        
        wl_compositor* get_compositor() const { return compositor.get(); }
        wl_subcompositor* get_subcompositor() const { return subcompositor.get(); }
        xdg_wm_base* get_shell() const { return shell.get(); }
        wl_shm* get_shm() const { return shm.get(); }
        bool is_keyboard_available() const { return keyboard != nullptr; }
        wl_pointer* get_pointer() const { return pointer.get(); }
        xkb_context* get_xkb_context() const { return kb_context.get(); }
        xkb_state* get_state() const { return kb_state.get(); }
        
        void set_compositor(wl_compositor* compositor) { this->compositor = WlCompositorPtr(compositor); }
        void set_subcompositor(wl_subcompositor* subcompositor) { this->subcompositor = WlSubCompositorPtr(subcompositor); }
        void set_shell(xdg_wm_base* shell);
        void set_shm(wl_shm* shm) { this->shm = WlShmPtr(shm); }
        void set_seat(wl_seat* seat);
        void set_keyboard(wl_keyboard* keyboard) { this->keyboard = WlKeyboardPtr(keyboard); }
        void unset_keyboard() { this->keyboard.reset(); }
        void set_pointer(wl_pointer* pointer) { this->pointer = WlPointerPtr(pointer); }
        void unset_pointer() { this->pointer.reset(); }
        void set_keymap(xkb_keymap* keymap) { this->kb_keymap = XkbKeymapPtr(keymap); }
        void set_kb_state(xkb_state* state) { this->kb_state = XkbStatePtr(state); }

        
        std::shared_ptr<WaylandWindow> create_window(WindowProperties properties)
        {
            return std::make_shared<WaylandWindow>(properties);
        }
        
        bool flush();
        bool update();
        void clear();

    private:
        
        static void seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities);
        static void seat_name(void* data, struct wl_seat* seat, const char* name);
        static void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial);
        
        static constexpr wl_seat_listener seat_listener
        {
            &WaylandClient::seat_capabilities,
            &WaylandClient::seat_name
        };
        static constexpr xdg_wm_base_listener shell_listener
        {
            &WaylandClient::shell_ping
        };

        void on_shell_ping(xdg_wm_base *shell, uint32_t serial);
        void on_seat_capabilities(wl_seat *seat, uint32_t capabilities);
        void on_seat_name(wl_seat *seat, const char *name); 

        WaylandClient();

        void initialize();

        WaylandDisplay display;
        std::unique_ptr<WaylandRegistry> registry;

        WlCompositorPtr compositor;

        WlSubCompositorPtr subcompositor;
        XdgShellPtr shell;
        WlShmPtr shm;
        WlSeatPtr seat;
        WlKeyboardPtr keyboard;
        WlPointerPtr pointer;
        XkbContextPtr kb_context;
        XkbKeymapPtr kb_keymap;
        XkbStatePtr kb_state;
};

}