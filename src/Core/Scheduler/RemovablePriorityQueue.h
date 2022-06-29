#ifndef REMOVABLE_PRIORITY_QUEUE_H
#define REMOVABLE_PRIORITY_QUEUE_H

#include <queue>
#include <vector>

#include "../Event/Event.h"

class RemovablePriorityQueue 
    : public std::priority_queue <Event*, std::vector<Event*>, EventPointerComp> {
public:
    bool remove(const Event* evt) {
	auto itElement = std::find(this->c.begin(), this->c.end(), evt);
	while (itElement != this->c.end()) {
	    this->c.erase(itElement);
	    std::make_heap(this->c.begin(), this->c.end(), this->comp);
	    return true;
	}
	return false;
    }
};

#endif