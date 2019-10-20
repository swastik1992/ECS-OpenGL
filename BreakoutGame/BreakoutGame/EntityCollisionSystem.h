#pragma once
#include "BreakoutWorld.h"
#include "Utils.h"
#include "SystemContainers.h"

//===== Collision System ======//

typedef std::tuple<GLboolean, Direction, glm::vec2> CollisionParam; // <collision?, what direction?, difference vector center - closest point>

class EntityCollisionSystem : public System
{
public:
	EntityCollisionSystem();
	virtual void Update(float dt);

protected:

	GLboolean CheckCollisionBoxes(Entity& boxA, Entity& boxB);
	CollisionParam CheckCollisionCircleAndBox(Entity& circle, Entity& box);
	Direction VectorDirection(glm::vec2 closest);
};