#pragma once

#include <memory>

#include "wayland_types.hpp"

namespace tobi_engine
{
class WaylandWindow;

class WaylandClient
{
    public:
        static std::shared_ptr<WaylandClient> get_instance();
        
        ~WaylandClient();

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
        void set_pointer(WlPointerPtr pointer) { this->pointer = std::move(pointer); }
        void unset_pointer() { this->pointer.reset(); }
        void set_keymap(xkb_keymap* keymap) { this->kb_keymap = XkbKeymapPtr(keymap); }
        void set_kb_state(xkb_state* state) { this->kb_state = XkbStatePtr(state); }

        
        bool flush();
        bool update();
        void clear();

    private:

        static std::shared_ptr<WaylandClient> instance;

        WaylandClient();

        void initialize();


        WlDisplayPtr display;
        WlRegistryPtr registry;

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