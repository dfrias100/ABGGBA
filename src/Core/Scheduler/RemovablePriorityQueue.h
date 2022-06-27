#ifndef REMOVABLE_PRIORITY_QUEUE_H
#define REMOVABLE_PRIORITY_QUEUE_H

#include <queue>
#include <vector>

#include "../Event/Event.h"

class RemovablePriorityQueue 
    : public std::priority_queue <Event, std::vector<Event>, std::greater<Event>> {
public:
    bool remove(const Event& evt) {
	std::vector<Event>::iterator itElem;
	std::vector<Event>::iterator itHeapSearchBegin = this->c.begin();
	while ((itElem = std::find(itHeapSearchBegin, this->c.end(), evt)) != this->c.end()) {
	    if (itElem->m_EvtType == evt.m_EvtType) {
		this->c.erase(itElem);
		std::make_heap(this->c.begin(), this->c.end(), this->comp);
		return true;
	    }
	    itHeapSearchBegin = ++itElem;
	}
	return false;
    }
};

#endif