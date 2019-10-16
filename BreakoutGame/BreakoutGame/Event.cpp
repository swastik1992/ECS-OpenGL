#include "Event.h"
#include "World.h"

BaseEvent::Id BaseEvent::nextId = 0;

void EventManager::destroyEvents()
{	
	std::cout << "Event Destroyed" << std::endl;
	for (auto &it : eventPools) {
		auto pool = it.second;
		pool->clear();
	}
}


