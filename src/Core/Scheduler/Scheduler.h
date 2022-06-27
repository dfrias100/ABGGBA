#ifndef SCHEDULER_H
#define SCHEDULER_H


#include "RemovablePriorityQueue.h"
#include "../Event/Event.h"

class Scheduler {
    RemovablePriorityQueue m_qEvents;
    Event m_IdleEvent;
public:
    Scheduler();
    bool AreThereEvents();
    void DoEvent();
    void ScheduleEvent(Event& evt, uint64_t ulTriggerTime);
    void RemoveEvent(Event& evt);
    static uint64_t m_ulSystemClock;
};

#endif