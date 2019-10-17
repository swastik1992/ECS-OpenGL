#pragma once
#include "BreakoutWorld.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "SystemContainers.h"

//===== Collision System ======//

// Represents the four possible (collision) directions
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

//typedef std::tuple<GLboolean, glm::vec2, glm::vec2> CollisionParam;
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>

class EntityCollisionSystem : public System
{
public:
	EntityCollisionSystem();
	virtual void Update(float dt);

protected:

	GLboolean CheckCollisionBoxes(Entity& boxA, Entity& boxB);
	Collision CheckCollisionCircleAndBox(Entity& circle, Entity& box);
	void HandleCollisionCircleAndBox(Entity& circle, Collision& collisionParam);
	void HandlePlayerCollision(Entity& circle, Entity& player);
	Direction VectorDirection(glm::vec2 closest);


	//CollisionParam CheckCollision(Entity& a, Entity& b);
	//CollisionParam CheckCollisionCircleAndBox(Entity& circle, Entity& box);
	//CollisionParam CheckCollisionCircles(Entity& circleA, Entity& circleB);
	//CollisionParam CheckCollisionBoxes(Entity& boxA, Entity& boxB);

	//std::tuple<glm::vec2, glm::vec2> HitNormalDirection(glm::vec2 direction);
};