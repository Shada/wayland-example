#pragma once

#include <memory>

#include "wayland_deleters.hpp"

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
        
        void set_compositor(wl_compositor* compositor) { this->compositor = CompositorPtr(compositor); }
        void set_subcompositor(wl_subcompositor* subcompositor) { this->subcompositor = SubCompositorPtr(subcompositor); }
        void set_shell(xdg_wm_base* shell);
        void set_shm(wl_shm* shm) { this->shm = ShmPtr(shm); }
        void set_seat(wl_seat* seat);
        void set_keyboard(wl_keyboard* keyboard) { this->keyboard = KeyboardPtr(keyboard); }
        void set_pointer(wl_pointer* pointer) { this->pointer = PointerPtr(pointer); }
        void set_keymap(xkb_keymap* keymap) { this->kb_keymap = KbKeymapPtr(keymap); }
        void set_kb_state(xkb_state* state) { this->kb_state = KbStatePtr(state); }

        void flush();
        void update();
        void clear();

    private:

        static std::shared_ptr<WaylandClient> instance;

        WaylandClient();

        void initialize();

        DisplayPtr display;
        RegistryPtr registry;

        CompositorPtr compositor;

        SubCompositorPtr subcompositor;
        XdgShellPtr shell;
        ShmPtr shm;
        SeatPtr seat;
        KeyboardPtr keyboard;
        PointerPtr pointer;
        KbContextPtr kb_context;
        KbKeymapPtr kb_keymap;
        KbStatePtr kb_state;
};

}