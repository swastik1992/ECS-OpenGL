#pragma once
#include "GameComponents.h"
#include "System.h"
#include "GameEvents.h"
#include "BreakoutWorld.h"
#include "EntityCollisionSystem.h"
#include "Utils.h"

//===== Movement System ======//

typedef std::tuple<GLboolean, Direction, glm::vec2> CollisionParam; // <collision?, what direction?, difference vector center - closest point>

class MovementSystem : public System
{
public:
	MovementSystem();
	virtual void Update(float dt);
	virtual void Init();
	void OnCollisionEvent(CollisionEvent* event);

protected:
	void HandleCollisionCircleAndBox(Entity& circle, CollisionParam const& collisionParam);
	void HandlePlayerCollision(Entity& circle, Entity& player);
};
