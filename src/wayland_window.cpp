#include "wayland_window.hpp"

#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

#include "shared_memory.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "wayland_client.hpp"


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

namespace 
{
void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {

    auto window = (MyWindow*)data;
    xdg_surface_ack_configure(xdg_surface, serial);

    Logger::debug("xdg_surface_configure()");

    if(!window->is_configured()) {
        window->resize();
    }
    window->draw();
}

const struct xdg_surface_listener xdg_surface_listener = 
{
    .configure = xdg_surface_configure
};

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
    auto window = (MyWindow*)data;
    window->close_window();
}
const struct xdg_toplevel_listener toplevel_listener = 
{
    .configure = toplevel_configure,
    .close = toplevel_close
};

void frame_new(void* data, struct wl_callback* callback, uint32_t callback_data);

const struct wl_callback_listener callback_listener = 
{
    .done = frame_new
};


void frame_new(void* data, struct wl_callback* callback, uint32_t callback_data) {

    auto window = (MyWindow*)data;
    wl_callback_destroy(callback);
    callback = wl_surface_frame(window->get_surface());
    wl_callback_add_listener(callback, &callback_listener, window);
    
    window->set_callback(callback);

    window->draw();
}
    
}

void MyWindow::update()
{
    client->update();
}

void MyWindow::initialize()
{
    surface = wl_compositor_create_surface(client->get_compositor());

    callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &callback_listener, this);

    x_surface = xdg_wm_base_get_xdg_surface(client->get_shell(), surface);
    xdg_surface_add_listener(x_surface, &xdg_surface_listener, this);

    x_toplevel = xdg_surface_get_toplevel(x_surface);
    xdg_toplevel_add_listener(x_toplevel, &toplevel_listener, this);
    xdg_toplevel_set_title(x_toplevel, "My test window: Wayland");

    wl_surface_commit(surface);

}

MyWindow::MyWindow()
    :   client(WaylandClient::get_instance()),
        shared_memory(std::make_shared<SharedMemory>(0))
{
    initialize();
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
    Logger::debug("resize()");

    const uint32_t buffer_size = width * height * 4;
    shared_memory->resize(buffer_size);

    create_shared_memory();
    create_buffer();
}

void MyWindow::create_shared_memory()
{
    const uint32_t buffer_size = width * height * 4;
    pixels = static_cast<uint8_t*>(mmap(nullptr, buffer_size,
                                                PROT_READ | PROT_WRITE,
                                                MAP_SHARED, shared_memory->get_fd(), 0));
    if (pixels == MAP_FAILED)
    {
        throw std::runtime_error("Failed to map shared memory.");
    }
}

void MyWindow::create_buffer()
{
    const uint32_t buffer_size = width * height * 4;
    wl_shm_pool* pool = wl_shm_create_pool(client->get_shm(), shared_memory->get_fd(), buffer_size);
    if (!pool)
    {
        munmap(pixels, buffer_size);
        throw std::runtime_error("Failed to create Wayland SHM pool.");
    }
    buffer = wl_shm_pool_create_buffer(pool, 0, width, height, width * 4, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    if (!buffer)
    {
        munmap(pixels, buffer_size);
        throw std::runtime_error("Failed to create Wayland buffer.");
    }
}

void MyWindow::draw()
{   
    colour++;
    //std::memset(pixels, colour, width*height*4);
    std::fill(pixels, pixels + width*height*4, colour);
    //memset(pixels, colour, width*height*4);
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, width, height);
    wl_surface_commit(surface);
}

void MyWindow::destroy()
{
    if(pixels && width > 0 && height > 0) 
        munmap(pixels, width * height * 4);
    if (buffer)
        wl_buffer_destroy(buffer);
    if(x_toplevel) 
        xdg_toplevel_destroy(x_toplevel);
    if(x_surface) 
        xdg_surface_destroy(x_surface);
    if(callback) 
        wl_callback_destroy(callback);
    if(surface) 
        wl_surface_destroy(surface);
}

} // namespace tobi_engine
