#pragma once

#include <random>

namespace tobi_engine
{
        
    inline
    std::string generate_random_string(uint32_t length = 10)
    {
        static const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device random_device;
        thread_local static std::mt19937 generator(std::random_device{}());
        std::uniform_int_distribution<> distribution(0, characters.size() - 1);

        std::string random_string;

        for(size_t i = 0; i < length; i++)
        {
            random_string += characters[distribution(generator)];
        }
        return random_string;
    }


    namespace 
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

    }
}