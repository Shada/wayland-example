#pragma once

#include "event.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
namespace tobi_engine
{

    class EventRegistry
    {
    public:
        EventRegistry() = default;
        EventRegistry(const EventRegistry&) = delete;
        EventRegistry(EventRegistry&&) = delete;
        EventRegistry& operator=(const EventRegistry&) = delete;
        EventRegistry& operator=(EventRegistry&&) = delete;
        ~EventRegistry();

        // Register an event with a specific type and data.
        //void register_event(EventType type, const Event& event);
        // Deregister an event by its type.
        //void deregister_event(EventType type, const Event& event);
        // Trigger an event, calling all registered callbacks for that event type.
        //void trigger_event(EventType type, const Event& event);
        // Add a callback for a specific event type.
        //void add_event_callback(EventType type, std::function<void(const Event&)> callback);
        // Remove a callback for a specific event type.
        //void remove_event_callback(EventType type, std::function<void(const Event&)> callback);
        // Clear all registered events.
        //void clear_events();
        // Get the current state of the event system, such as the number of registered events.
        //size_t get_event_count() const { return events.size(); }
        // Get the current event being processed, if any.
        //const Event* get_current_event() const 
        //{
        //    if(events.empty())
        //        return nullptr;
        //    return &events.back(); // Return the last event as the current event
        //}
        // Check if an event of a specific type is registered.
        //bool is_event_registered(EventType type) const
        //{
        //    return event_callbacks.find(type) != event_callbacks.end();
        //}

    private:
        std::vector<Event> events;

        // This will be used to store the callbacks for each event type.
        std::unordered_map<EventType, std::vector<std::function<void(const Event&)>>> event_callbacks;
    };
}