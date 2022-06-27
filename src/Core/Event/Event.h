#ifndef EVENT_H
#define EVENT_H

#include <limits>
#include <functional>

#include <cstdint>

enum class EventType {
    Idle,
    H_Blank,
    H_BlankEnd,
    DMA
};

struct Event {
    uint16_t m_ulTriggerTime = std::numeric_limits<uint64_t>::max();
    std::function<void(uint64_t)> m_Callback;
    EventType m_EvtType = EventType::Idle;
};

inline bool operator<(const Event& leftEvent, const Event& rightEvent) {
    return leftEvent.m_ulTriggerTime < rightEvent.m_ulTriggerTime;
}

inline bool operator>(const Event& leftEvent, const Event& rightEvent) {
    return leftEvent.m_ulTriggerTime > rightEvent.m_ulTriggerTime;
}

inline bool operator>=(const Event& leftEvent, const Event& rightEvent) {
    return !(leftEvent < rightEvent);
}

inline bool operator<=(const Event& leftEvent, const Event& rightEvent) {
    return !(leftEvent > rightEvent);
}

inline bool operator==(const Event& leftEvent, const Event& rightEvent) {
    return leftEvent.m_ulTriggerTime == rightEvent.m_ulTriggerTime;
}

inline bool operator!=(const Event& leftEvent, const Event& rightEvent) {
    return !(leftEvent == rightEvent);
}

#endif