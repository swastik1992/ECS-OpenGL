#pragma once

#include "Config.h"
#include <bitset>
#include <cstdint>
#include <cassert>

struct BaseComponent
{
	using Id = uint8_t;
	static const Id MaxComponents = MAX_COMPONENTS;
protected:
	static Id nextCompId;
};

// Used to assign a unique id to a component type, we don't really have to make our components derive from this though.
template <typename T>
struct Component : BaseComponent
{
	// Returns the unique id of Component<T>
	static Id getId()
	{
		static auto id = nextCompId++;
		assert(id < MaxComponents);
		return id;
	}
};

// Used to keep track of which components an entity has and also which entities a system is interested in.
using ComponentMask = std::bitset<BaseComponent::MaxComponents>;


