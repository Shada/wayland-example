#pragma once

#include <cstdint>
namespace tobi_engine
{

enum class EventType 
{ 
    KeyboardEnter,
    KeyboardLeave,
    PointerEnter,
    PointerLeave,
    KeyPress,
    KeyRelease,
    PointerButtonPress,
    PointerButtonRelease,
    PointerMotion,
    WindowResize,
    WindowClose,
    Custom // For any other event types
};

class Event
{
public:
    Event() = default;
    Event(const Event&) = default;
    Event(Event&&) = default;
    Event& operator=(const Event&) = default;
    Event& operator=(Event&&) = default;
    virtual ~Event() = default;

private:
    EventType type;
    uint64_t window_uid; // UID of the window this event is associated with
    // Additional data can be added here, such as key codes, mouse positions, etc.

};

}
