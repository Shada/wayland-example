#pragma once

#include "event.hpp"

struct wl_pointer;

namespace tobi_engine
{

class PointerEvent : public Event
{
public:
    PointerEvent() = delete; // Default constructor is deleted to ensure proper initialization
    PointerEvent(int32_t x, int32_t y, uint32_t button, uint32_t state, uint32_t serial, wl_pointer* pointer);
    PointerEvent(const PointerEvent&) = default;
    PointerEvent(PointerEvent&&) = default;
    PointerEvent& operator=(const PointerEvent&) = default;
    PointerEvent& operator=(PointerEvent&&) = default;
    virtual ~PointerEvent() = default;  
    int32_t get_x() const { return position.x; }
    int32_t get_y() const { return position.y; }
    uint32_t get_button() const { return button; }
    uint32_t get_state() const { return state; }
    uint32_t get_serial() const { return serial; }
    wl_pointer* get_pointer() const { return pointer; }
private:
    struct Position
    {
        int32_t x = 0;
        int32_t y = 0;
    } position;
    uint32_t button = 0; // Button pressed
    uint32_t state = 0; // 0 for released, 1 for pressed
    uint32_t serial = 0; // Serial number for the event
    wl_pointer* pointer; // Pointer object associated with the event
    
};

}