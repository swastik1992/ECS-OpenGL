#pragma once

#include "Pool.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <cstdint>
#include <iostream>


class World;

struct BaseEvent
{
	using Id = uint8_t;
protected:
	static Id nextId;
};

template <typename T>
struct Event : BaseEvent
{
	// Returns the unique id of Event<T>
	static Id getId()
	{
		static auto id = nextId++;
		return id;
	}
};

class EventManager
{
public:
	EventManager(World &world) : world(world) {}

	template <typename T>
	void emitEvent(T event);

	template <typename T, typename ... Args>
	void emitEvent(Args && ... args);

	template <typename T>
	std::vector<T> getEvents();

	void destroyEvents();

private:
	template <typename T>
	std::shared_ptr<Pool<T>> accommodateEvent();

	std::unordered_map<std::type_index, std::shared_ptr<AbstractPool>> eventPools;

	World &world;
};

template <typename T>
void EventManager::emitEvent(T event)
{
	std::cout << "Event Added for: " << std::type_index(typeid(T)).name() << std::endl;
	std::shared_ptr<Pool<T>> eventPool = accommodateEvent<T>();
	eventPool->add(event);
}

template <typename T, typename ... Args>
void EventManager::emitEvent(Args && ... args)
{
	T event(std::forward<Args>(args) ...);
	emitEvent<T>(event);
}

template <typename T>
std::shared_ptr<Pool<T>> EventManager::accommodateEvent()
{
	if (eventPools.find(std::type_index(typeid(T))) == eventPools.end()) {
		std::shared_ptr<Pool<T>> pool(new Pool<T>());
		eventPools.insert(std::make_pair(std::type_index(typeid(T)), pool));
	}

	return std::static_pointer_cast<Pool<T>>(eventPools[std::type_index(typeid(T))]);
}

template <typename T>
std::vector<T> EventManager::getEvents()
{
	//std::cout << "Somthing: " << std::type_index(typeid(T)).hash_code() <<std::endl;

	if (eventPools.size() > 0)
	{
		
		if(eventPools.find(std::type_index(typeid(T))) == eventPools.end())
			return std::vector<T>();
		else
		{
			std::cout << "Event Looking for: " << std::type_index(typeid(T)).name() << std::endl;
			std::cout << "Number of Data: " << std::static_pointer_cast<Pool<T>>(eventPools[std::type_index(typeid(T))]).get()->getData().size() << std::endl;

			return std::static_pointer_cast<Pool<T>>(eventPools[std::type_index(typeid(T))]).get()->getData();
		}
	}
	else
		return std::vector<T>();
}


