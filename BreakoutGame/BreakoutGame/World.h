// Mix - A minimal entity-component-system.
// Copyright 2014-2018 Par Arvidsson. All rights reserved.
// Licensed under the MIT license (https://github.com/arvidsson/Mix/blob/master/LICENSE).

#pragma once

#include "Entity.h"
#include "System.h"
#include "Event.h"
#include <vector>
#include <string>
#include <memory>
#include "Utils.h"

class World
{
public:
	World(GLint _width = 800, GLint _height = 600);
	virtual ~World(){}

	EntityManager& getEntityManager() const;
	SystemManager& getSystemManager() const;
	EventBus& getEventBus() const;

	virtual void Init();
	
	virtual void Update(float dt);

	virtual void Render();

	Entity createEntity();
	void destroyEntity(Entity e);

	Entity getEntity(std::string tag) const;
	std::vector<Entity> getGroup(std::string group) const;

	GLint height, width;

protected:
	// vector of entities that are awaiting creation
	std::vector<Entity> createdEntities;

	// vector of entities that are awaiting destruction
	std::vector<Entity> destroyedEntities;

	std::unique_ptr<EntityManager> entityManager = nullptr;
	std::unique_ptr<SystemManager> systemManager = nullptr;
	std::unique_ptr<EventBus> eventBus = nullptr;
};

