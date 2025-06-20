#pragma once

#include <iostream>
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

            struct wl_display* get_display() { return display; };
            struct wl_registry* get_registry() { return registry; };

            struct wl_compositor* get_compositor() { return compositor; };
            struct xdg_wm_base* get_shell() { return shell; };
            struct wl_shm* get_shm() { return shm; };
            
            void set_compositor(struct wl_compositor* compositor) { this->compositor = compositor; };
            void set_shell(struct xdg_wm_base* shell) { this->shell = shell; };
            void set_shm(struct wl_shm* shm) { this->shm = shm; };

            void update();

            struct wl_compositor* compositor = nullptr;
            struct xdg_wm_base* shell = nullptr;
            struct wl_shm* shm = nullptr;
        private:

            WaylandClient();

            void initialize();

            struct wl_display* display = nullptr;
            struct wl_registry* registry = nullptr;
    };

}