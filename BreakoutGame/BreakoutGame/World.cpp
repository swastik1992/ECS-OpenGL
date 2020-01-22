#include "World.h"
#include <cassert>


World::World(GLint _width, GLint _height)
	:width(_width), height(_height)
{
	entityManager = std::make_unique<EntityManager>(*this);
	systemManager = std::make_unique<SystemManager>(*this);
	eventBus = std::make_unique<EventBus>();
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


