#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <queue>
#include <functional>

class EventSystem {
private:
    std::queue<std::function<void()>> eventQueue;

public:
    EventSystem();

    // Add event to queue
    void trigger(std::function<void()> func);

    // Process all queued events
    void processEvents();

    // Optional: clear all events
    void clear();
};

#endif