#pragma once

#include <memory>
#include <unordered_map>

#include "wayland_deleters.hpp"

struct zxdg_decoration_manager_v1;

namespace tobi_engine
{
class WaylandWindow;

class WaylandClient
{
    public:
        static std::shared_ptr<WaylandClient> get_instance();
        
        ~WaylandClient();

        wl_compositor* get_compositor() const { return compositor.get(); }
        xdg_wm_base* get_shell() const { return shell.get(); }
        wl_shm* get_shm() const { return shm.get(); }
        wl_keyboard* get_keyboard() const { return keyboard.get(); }
        
        void set_compositor(wl_compositor* compositor) { this->compositor = CompositorPtr(compositor); }
        void set_subcompositor(wl_subcompositor* subcompositor) { this->subcompositor = SubCompositorPtr(subcompositor); }
        void set_shell(xdg_wm_base* shell);
        void set_shm(wl_shm* shm) { this->shm = ShmPtr(shm); }
        void set_seat(wl_seat* seat);
        void set_keyboard(wl_keyboard* keyboard) { this->keyboard = KeyboardPtr(keyboard); }

        void update();

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
};

}