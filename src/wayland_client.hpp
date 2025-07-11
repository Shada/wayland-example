#pragma once

//#include "wayland_input_manager.hpp"
#include "wayland_input_manager.hpp"
#include "wayland_types.hpp"
#include "wayland_window.hpp"
#include "wayland_display.hpp"
#include "wayland_registry.hpp"
#include <memory>

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
        bool is_keyboard_available() const { return wayland_input_manager->get_keyboard() != nullptr; }
        wl_pointer* get_pointer() const { return wayland_input_manager->get_pointer(); }
        //xkb_context* get_xkb_context() const { return kb_context.get(); }
        xkb_state* get_state() const { return wayland_input_manager->get_kb_state(); }
        
        void reset_compositor() { this->compositor.reset(); }
        //void set_keyboard(wl_keyboard* keyboard) { this->keyboard = WlKeyboardPtr(keyboard); }
        //void unset_keyboard() { this->keyboard.reset(); }
        //void set_pointer(wl_pointer* pointer) { this->pointer = WlPointerPtr(pointer); }
        //void unset_pointer() { this->pointer.reset(); }
        //void set_keymap(xkb_keymap* keymap) { this->kb_keymap = XkbKeymapPtr(keymap); }
        //void set_kb_state(xkb_state* state) { this->kb_state = XkbStatePtr(state); }

        
        std::shared_ptr<WaylandWindow> create_window(WindowProperties properties)
        {
            return std::make_shared<WaylandWindow>(properties);
        }
        
        bool flush();
        bool update();
        void clear();

    private:
        
        void set_shell(xdg_wm_base* shell);

        static void shell_ping(void *data, xdg_wm_base *shell, uint32_t serial);

        static constexpr xdg_wm_base_listener shell_listener
        {
            &WaylandClient::shell_ping
        };

        void on_shell_ping(xdg_wm_base *shell, uint32_t serial);

        WaylandClient();

        void initialize();

        WaylandDisplay display;
        std::unique_ptr<WaylandRegistry> wayland_registry;
        std::unique_ptr<WaylandInputManager> wayland_input_manager;

        WlCompositorPtr compositor;

        WlSubCompositorPtr subcompositor;
        XdgShellPtr shell;
        WlShmPtr shm;
        WlSeatPtr seat;
        //WlKeyboardPtr keyboard;
        //WlPointerPtr pointer;
        //XkbContextPtr kb_context;
        //XkbKeymapPtr kb_keymap;
        //XkbStatePtr kb_state;
};

}