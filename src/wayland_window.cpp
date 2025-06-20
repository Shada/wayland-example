#include "wayland_window.hpp"

#include <cstring>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

#include <iostream>
#include <random>
#include <sstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "utils/logger.hpp"
#include "wayland_client.hpp"
#include "wayland_util.hpp"

#include "utils/utils.hpp"



namespace tobi_engine
{



//void keyboard_map(void *data, struct wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size) {
//
//    printf("keyboard_map()\n");
//}
//
//void keyboard_enter(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array* keys) {
//    
//    printf("keyboard_enter()\n");
//}
//
//void keyboard_leave(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface) {
//    
//    printf("keyboard_leave()\n");
//}
//
//void keyboard_key(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t t, uint32_t key, uint32_t status) {
//
//    printf("keyboard_key()\n");
//    if(key == 1) 
//        is_closed = 1;
//
//    if(key == 30)
//        xdg_toplevel_set_fullscreen(x_top, NULL);
//    if(key == 32)
//        xdg_toplevel_unset_fullscreen(x_top);
//}
//
//void keyboard_modifiers(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t group) {
//    
//    printf("keyboard_modifiers()\n");
//}
//
//void keyboard_repeat(void *data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) {
//    
//    printf("keyboard_repeat()\n");
//}
//
//static const struct wl_keyboard_listener keyboard_listener = {
//    .keymap = keyboard_map,
//    .enter = keyboard_enter,
//    .leave = keyboard_leave,
//    .key = keyboard_key,
//    .modifiers = keyboard_modifiers,
//    .repeat_info = keyboard_repeat
//};
//
//void seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) {
//
//    printf("seat_capabilities()\n");
//    if(capabilities & WL_SEAT_CAPABILITY_KEYBOARD && !keyboard) {
//        keyboard = wl_seat_get_keyboard(seat);
//        wl_keyboard_add_listener(keyboard, &keyboard_listener, 0);
//    }
//}
//void seat_name(void* data, struct wl_seat* seat, const char* name) {
//
//    printf("seat_name()\n");
//}
//
//static const struct wl_seat_listener seat_listener = {
//    .capabilities = seat_capabilities,
//    .name = seat_name
//};

void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {

    auto window = (MyWindow*)data;
    xdg_surface_ack_configure(xdg_surface, serial);

    Logger::debug("xdg_surface_configure()");

    if(!window->is_configured()) {
        window->resize();
    }
    window->draw();
}

void toplevel_configure(void *data, struct xdg_toplevel *toplevel, int32_t new_width, int32_t new_height, struct wl_array* states) {

    auto window = (MyWindow*)data;
    Logger::debug("toplevel_configure()");
    if(!new_height && !new_width) {
        return ;
    }
    window->resize(new_width, new_height);
}

void toplevel_close(void* data, struct xdg_toplevel *toplevel) {

    Logger::debug("toplevel_close()");
    is_closed = 1;
}


void frame_new(void* data, struct wl_callback* callback, uint32_t callback_data) {

    auto window = (MyWindow*)data;
    wl_callback_destroy(callback);
    callback = wl_surface_frame(window->get_surface());
    wl_callback_add_listener(callback, &callback_listener, window);
    
    window->set_callback(callback);

    colour++;
    window->draw();
}

class SharedMemory
{
    public:
        SharedMemory(uint32_t size) { resize(size); };
        ~SharedMemory() { close(file_descriptor); }

        int32_t get_fd() { return file_descriptor; }
        void resize(uint32_t size) 
        { 
            if(size == this->size) 
                return;
            close(file_descriptor);
            allocate_shm(size);
        }
    private:
        int32_t file_descriptor;
        uint32_t size;
        
        void allocate_shm(uint64_t size) 
        {
            // create random filename
            std::string name = "/window-handle-" + generate_random_string(10);

            Logger::debug("Created SHM: " + name);

            file_descriptor = memfd_create(name.c_str(), 0);
            if(file_descriptor == -1)
                throw std::runtime_error("Failed to open file " + name);
            
            shm_unlink(name.c_str());

            if(ftruncate(file_descriptor, size) == -1)
            {   
                close(file_descriptor);
                throw std::runtime_error("Failed to truncate file " + name);
            }
        }
};

void MyWindow::update()
{
    WaylandClient::get_instance()->update();
}

void MyWindow::init()
{
    auto wl_client = tobi_engine::WaylandClient::get_instance();

    surface = wl_compositor_create_surface(wl_client->get_compositor());

    callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &callback_listener, this);

    x_surface = xdg_wm_base_get_xdg_surface(wl_client->get_shell(), surface);
    xdg_surface_add_listener(x_surface, &xdg_surface_listener, this);

    x_top = xdg_surface_get_toplevel(x_surface);
    xdg_toplevel_add_listener(x_top, &toplevel_listener, this);
    xdg_toplevel_set_title(x_top, "My test window: Wayland");

    wl_surface_commit(surface);

}

void MyWindow::resize(uint16_t w, uint16_t h)
{
    if (width == w && height == h) 
        return;
    munmap(pixels, width * height* 4);
    width = w;
    height = h;
    resize();
}
void MyWindow::resize()
{
    auto client = WaylandClient::get_instance();
    Logger::debug("resize()");
    auto shared = SharedMemory(width * height * 4);
    int32_t fd = shared.get_fd();
    //int32_t fd = allocate_shm(width * height * 4);

    pixels = (uint8_t*)mmap(0, width * height * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct wl_shm_pool *pool = wl_shm_create_pool(client->get_shm(), fd, width * height * 4);
    if (!pool) {
        close(fd);
        throw std::runtime_error("Failed to create Wayland SHM pool");
    }
    buffer = wl_shm_pool_create_buffer(pool, 0, width, height, width * 4, WL_SHM_FORMAT_ARGB8888);
    if (!buffer) {
        wl_shm_pool_destroy(pool);
        close(fd);
        throw std::runtime_error("Failed to create Wayland buffer");
    }
    wl_shm_pool_destroy(pool);
    //close(fd);
}

void MyWindow::draw()
{   
    memset(pixels, colour, width*height*4);
    //memset(pixels, colour, width*height*4);
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, width, height);
    wl_surface_commit(surface);
}

void MyWindow::destroy()
{
    if(pixels) {
        munmap(pixels, width * height * 4);
    }
    if(buffer) {
        wl_buffer_destroy(buffer);
    }
    if(x_top) xdg_toplevel_destroy(x_top);
    if(x_surface) xdg_surface_destroy(x_surface);
    if(callback) wl_callback_destroy(callback);
    if(surface) wl_surface_destroy(surface);
}

} // namespace tobi_engine
