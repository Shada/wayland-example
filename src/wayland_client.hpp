#pragma once

#include <memory>

struct wl_display;
struct wl_registry;
struct wl_compositor;
struct xdg_wm_base;
struct wl_shm;

namespace tobi_engine
{

    class WaylandClient
    {
        static std::shared_ptr<WaylandClient> instance;

        public:
            static std::shared_ptr<WaylandClient> get_instance();
            
            ~WaylandClient();

            wl_display* get_display() { return display; };
            wl_registry* get_registry() { return registry; };

            wl_compositor* get_compositor() { return compositor; };
            xdg_wm_base* get_shell() { return shell; };
            wl_shm* get_shm() { return shm; };
            
            void set_compositor(wl_compositor* compositor) { this->compositor = compositor; };
            void set_shell(xdg_wm_base* shell) { this->shell = shell; };
            void set_shm(wl_shm* shm) { this->shm = shm; };

            void update();

        private:

            WaylandClient();

            void initialize();

            wl_display* display;
            wl_registry* registry;

            wl_compositor* compositor;
            xdg_wm_base* shell;
            wl_shm* shm;
    };

}