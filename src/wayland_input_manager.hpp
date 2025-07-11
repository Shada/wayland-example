#pragma once

#include "wayland_registry.hpp"
#include "wayland_types.hpp"

namespace tobi_engine
{

    /**
     * @class WaylandInputManager
     * @brief Manages input devices in the Wayland environment.
     * 
     * This class handles input devices such as keyboard and pointer, providing
     * functionality to manage their state and events.
     */
    class WaylandInputManager
    {
    public:
        explicit WaylandInputManager(WaylandRegistry& registry);
        ~WaylandInputManager() = default;
        WaylandInputManager(const WaylandInputManager&) = delete;
        WaylandInputManager& operator=(const WaylandInputManager&) = delete;
        WaylandInputManager(WaylandInputManager&&) = default;
        WaylandInputManager& operator=(WaylandInputManager&&) = default;

        /**
         * @brief Get the pointer to the Wayland seat.
         * @return Pointer to the Wayland seat.
         */
        wl_seat* get_seat() const noexcept { return seat.get(); }

        /**
         * @brief Get the pointer to the Wayland pointer device.
         * @return Pointer to the Wayland pointer device, or nullptr if not available.
         */
        wl_pointer* get_pointer() const noexcept { return pointer.get(); }
        /**
         * @brief Get the pointer to the Wayland keyboard device.
         * @return Pointer to the Wayland keyboard device, or nullptr if not available.
         */
        wl_keyboard* get_keyboard() const noexcept { return keyboard.get(); }
        /**
         * @brief Get the XKB context.
         * @return Pointer to the XKB context.
         */
        xkb_context* get_xkb_context() const noexcept { return kb_context.get(); }
        /**
         * @brief Get the XKB keymap.
         * @return Pointer to the XKB keymap, or nullptr if not available.
         */
        xkb_keymap* get_keymap() const noexcept { return kb_keymap.get(); }
        /**
         * @brief Get the XKB state.
         * @return Pointer to the XKB state, or nullptr if not available.
         */
        xkb_state* get_kb_state() const noexcept { return kb_state.get(); }

        /**
         * @brief Set the XKB keymap.
         * @param keymap Pointer to the XKB keymap.
         */
        void set_keymap(xkb_keymap* keymap) { kb_keymap = XkbKeymapPtr(keymap); }
        /**
         * @brief Set the XKB state.
         * @param state Pointer to the XKB state.
         */
        void set_kb_state(xkb_state* state) { kb_state = XkbStatePtr(state); }
        
    private:

        WlSeatPtr seat;
        WlPointerPtr pointer;
        WlKeyboardPtr keyboard;
        XkbContextPtr kb_context;
        XkbKeymapPtr kb_keymap;
        XkbStatePtr kb_state;

        static void seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities);
        static void seat_name(void* data, struct wl_seat* seat, const char* name);
        
        void on_seat_capabilities(wl_seat *seat, uint32_t capabilities);
        void on_seat_name(wl_seat *seat, const char *name); 

        static inline constexpr wl_seat_listener seat_listener
        {
            &WaylandInputManager::seat_capabilities,
            &WaylandInputManager::seat_name
        };

        static void pointer_enter(void *data, wl_pointer* pointer, uint32_t serial, wl_surface *surface, wl_fixed_t x, wl_fixed_t y);
        static void pointer_leave(void *data, wl_pointer* poiner, uint32_t serial, wl_surface *surface);
        static void pointer_motion(void *data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
        static void pointer_button(void *data, wl_pointer* poiner, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
        static void pointer_axis(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value);

        static inline constexpr wl_pointer_listener pointer_listener = 
        {
            &WaylandInputManager::pointer_enter,
            &WaylandInputManager::pointer_leave,
            &WaylandInputManager::pointer_motion,
            &WaylandInputManager::pointer_button,
            &WaylandInputManager::pointer_axis,
        };


        static void keyboard_map(void *data, struct wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size);
        static void keyboard_enter(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array* keys);
        static void keyboard_leave(void *data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface *surface);
        static void keyboard_key(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
        static void keyboard_modifiers(void *data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group);
        static void keyboard_repeat(void *data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay);

        static inline constexpr wl_keyboard_listener keyboard_listener = 
        {
            &WaylandInputManager::keyboard_map,
            &WaylandInputManager::keyboard_enter,
            &WaylandInputManager::keyboard_leave,
            &WaylandInputManager::keyboard_key,
            &WaylandInputManager::keyboard_modifiers,
            &WaylandInputManager::keyboard_repeat
        };

    };

}