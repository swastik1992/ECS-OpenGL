#pragma once

#include <bitset>
#include <cstdint>
#include <cassert>


struct BaseComponent
{
	using Id = uint8_t;
	static const Id MaxComponents = 64;
protected:
	static Id nextCompId;
};

using ComponentMask = std::bitset<BaseComponent::MaxComponents>;

template <typename T>
struct Component : BaseComponent
{
	static Id getId()
	{
		static auto id = nextCompId++;
		assert(id < MaxComponents);
		return id;
	}
};



