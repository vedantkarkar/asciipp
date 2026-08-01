#include "EventSystem.h"

EventSystem::EventSystem() {}

void EventSystem::trigger(std::function<void()> func) {
    eventQueue.push(func);
}

void EventSystem::processEvents() {
    while (!eventQueue.empty()) {
        auto event = eventQueue.front();
        eventQueue.pop();

        if (event) {
            event();
        }
    }
}

void EventSystem::clear() {
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
}