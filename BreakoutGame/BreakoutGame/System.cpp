#include "System.h"
#include "World.h"
#include <algorithm>

void System::addEntity(Entity e)
{
	entities.push_back(e);
}

void System::removeEntity(Entity e)
{
	entities.erase(std::remove_if(entities.begin(), entities.end(),
		[&e](Entity other) { return e == other; }
	), entities.end());
}

World& System::getWorld() const
{
	assert(world != nullptr);
	return *world;
}

void SystemManager::addToSystems(Entity e)
{
	const auto &entityComponentMask = world.getEntityManager().getComponentMask(e);

	for (auto &it : systems) {
		auto &system = it.second;
		const auto &systemComponentMask = system->getComponentMask();
		auto interest = (entityComponentMask & systemComponentMask) == systemComponentMask;

		if (interest) {
			system->addEntity(e);
		}
	}
}

void SystemManager::removeFromSystems(Entity e)
{
	for (auto &it : systems) {
		auto &system = it.second;
		system->removeEntity(e);
	}
}

void SystemManager::Update(float dt)
{
	std::unordered_map<std::type_index, std::shared_ptr<System>>::iterator it = systems.begin();

	// Iterate over the map using iterator
	while (it != systems.end())
	{
		it->second->Update(dt);
		it++;
	}
}

void SystemManager::Render()
{
	std::unordered_map<std::type_index, std::shared_ptr<System>>::iterator it = systems.begin();

	// Iterate over the map using iterator
	while (it != systems.end())
	{
		it->second->Render();
		it++;
	}
}



