#include "Scheduler.h"

Scheduler::Scheduler() {
    m_IdleEvent.m_Callback = [&](uint64_t ulLateTicks) {
	return;
    };
    m_qEvents.push(&m_IdleEvent);
}

bool Scheduler::AreThereEvents() {
    return Scheduler::m_ulSystemClock >= m_qEvents.top()->m_ulTriggerTime;
}

void Scheduler::DoEvent() {
    Event* pEvt = m_qEvents.top();
    pEvt->m_Callback(Scheduler::m_ulSystemClock - pEvt->m_ulTriggerTime);
    m_qEvents.pop();
}

void Scheduler::ScheduleEvent(Event& evt, uint64_t ulCyclesInFuture) {
    evt.m_ulTriggerTime = Scheduler::m_ulSystemClock + ulCyclesInFuture;
    m_qEvents.push(&evt);
}

void Scheduler::RemoveEvent(Event& evt) {
    evt.m_ulTriggerTime = std::numeric_limits<uint64_t>::max();
    m_qEvents.remove(&evt);
}
