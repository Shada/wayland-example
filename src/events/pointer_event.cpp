#include "pointer_event.hpp"


namespace tobi_engine
{

    PointerEvent::PointerEvent(int32_t x, int32_t y, uint32_t button, uint32_t state, uint32_t serial, wl_pointer* pointer)
        : position{x, y}, button(button), state(state), serial(serial), pointer(pointer)
    {}

}