#pragma once

#include "Pool.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <cstdint>
#include <iostream>
#include <map>

class World;


//============================== PUB|SUB EVENT MANAGER ====================================================
//


struct Event
{
	Event() {}
};

class FuntionHandler
{
public:

	void Fire(Event* event)
	{
		Execute(event);
	}

protected:

	virtual void Execute(Event* event) = 0;
};

template<class T, class Type>
class ECSFunctionHandler : public FuntionHandler
{
public:

	typedef void (T::*Function)(Type*);

	ECSFunctionHandler(T* _classInstance, Function _functionToCall)
		: classInstance(_classInstance), functionToCall(_functionToCall) {}

	void Execute(Event* event)
	{
		(classInstance->*functionToCall)(static_cast<Type*>(event));
	}

private:

	T* classInstance;

	Function functionToCall;
};

//-------------------

typedef std::list<FuntionHandler*> FunctionList;

class EventBus
{
public:

	template<class Type>
	void Publish(Type* event)
	{
		FunctionList *list = subscribers[typeid(Type)];

		if (list == nullptr) return;

		for (auto& funt : *list)
		{
			if (funt != nullptr) funt->Fire(event);
		}
	}

	template<class T, class Type>
	void Subscribe(T* classInstance, void (T::*function)(Type*))
	{
		FunctionList *list = subscribers[typeid(Type)];

		if (list == nullptr)
		{
			list = new FunctionList();
			subscribers[typeid(Type)] = list;
		}

		list->push_back(new ECSFunctionHandler<T,Type>(classInstance, function));
	}

private:

	std::map<std::type_index, FunctionList*> subscribers;
};


