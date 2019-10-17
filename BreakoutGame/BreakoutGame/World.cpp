#include "World.h"
#include <cassert>


World::World()
{
	entityManager = std::make_unique<EntityManager>(*this);
	systemManager = std::make_unique<SystemManager>(*this);
	eventManager = std::make_unique<EventManager>(*this);
	eventBus = std::make_unique<EventBus>();//todo
}

EntityManager& World::getEntityManager() const
{
	assert(entityManager != nullptr);
	return *entityManager;
}

SystemManager& World::getSystemManager() const
{
	assert(systemManager != nullptr);
	return *systemManager;
}

EventManager& World::getEventManager() const
{
	assert(eventManager != nullptr);
	return *eventManager;
}

EventBus& World::getEventBus() const
{
	assert(eventBus != nullptr);
	return *eventBus;
}

void World::Init()
{
	systemManager->Init();
}

void World::Update(float dt)
{
	for (auto e : createdEntities) {
		getSystemManager().addToSystems(e);
	}
	createdEntities.clear();

	for (auto e : destroyedEntities) {
		getSystemManager().removeFromSystems(e);
		getEntityManager().destroyEntity(e);
	}
	destroyedEntities.clear();
	
	systemManager->Update(dt);

	//getEventManager().destroyEvents();


}

void World::Render()
{
	systemManager->Render();
}


Entity World::createEntity()
{
	auto e = getEntityManager().createEntity();
	createdEntities.push_back(e);
	return e;
}

void World::destroyEntity(Entity e)
{
	destroyedEntities.push_back(e);
}

Entity World::getEntity(std::string tag) const
{
	return getEntityManager().getEntityByTag(tag);
}

std::vector<Entity> World::getGroup(std::string group) const
{
	return getEntityManager().getEntityGroup(group);
}


