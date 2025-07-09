#include "event_registry.hpp"

namespace tobi_engine
{

    EventRegistry::~EventRegistry()
    {
        //clear_events();
    }

    //void EventRegistry::register_event(EventType type, const Event& event)
    //{
    //    events.push_back(event);
    //    event_callbacks[type].push_back([event](const Event& e) { /* Handle the event */ });
    //}
//
    //void EventRegistry::deregister_event(EventType type, const Event& event)
    //{
    //    auto& callbacks = event_callbacks[type];
    //    callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), event), callbacks.end());
    //}
//
    //void EventRegistry::trigger_event(EventType type, const Event& event)
    //{
    //    for (const auto& callback : event_callbacks[type])
    //    {
    //        callback(event);
    //    }
    //}
//
    //void EventRegistry::add_event_callback(EventType type, std::function<void(const Event&)> callback)
    //{
    //    event_callbacks[type].push_back(callback);
    //}
//
    //void EventRegistry::remove_event_callback(EventType type, std::function<void(const Event&)> callback)
    //{
    //    auto& callbacks = event_callbacks[type];
    //    callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), callback), callbacks.end());
    //}
//
    //void EventRegistry::clear_events()
    //{
    //    events.clear();
    //    event_callbacks.clear();
    //}

}
