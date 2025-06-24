#pragma once

#include <memory>
#include <unordered_map>

struct wl_display;
struct wl_registry;
struct wl_compositor;
struct xdg_wm_base;
struct wl_shm;
struct wl_seat;
struct wl_keyboard;
struct wl_surface;

namespace tobi_engine
{
    class WaylandWindow;

    class WaylandClient
    {
        public:
            static std::shared_ptr<WaylandClient> get_instance();
            
            ~WaylandClient();

            wl_display* get_display() { return display; };
            wl_registry* get_registry() { return registry; };
            wl_compositor* get_compositor() { return compositor; };
            xdg_wm_base* get_shell() { return shell; };
            wl_shm* get_shm() { return shm; };
            wl_keyboard* get_keyboard() { return keyboard; };
            
            void set_compositor(wl_compositor* compositor) { this->compositor = compositor; };
            void set_shell(xdg_wm_base* shell) { this->shell = shell; };
            void set_shm(wl_shm* shm) { this->shm = shm; };
            void set_seat(wl_seat* seat) { this->seat = seat; };
            void set_keyboard(wl_keyboard* keyboard) { this->keyboard = keyboard; };

            void update();

        private:

            static std::shared_ptr<WaylandClient> instance;

            WaylandClient();

            void initialize();

            wl_display* display;
            wl_registry* registry;

            wl_compositor* compositor;
            xdg_wm_base* shell;
            wl_shm* shm;
            wl_seat* seat;
            wl_keyboard* keyboard;
    };

}