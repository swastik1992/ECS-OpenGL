#pragma once
#include "Event.h"
#include "Entity.h"
#include "Utils.h"
//=================///Events//============

struct InputEvent : public Event
{
	InputEvent(InputKeyAction _inputAction = Up, int _key = 0)
		:inputAction(_inputAction), key(_key) {}

	int key;
	InputKeyAction inputAction;
};

struct CollisionEvent : public Event
{
	CollisionEvent(Entity a = Entity(), Entity b = Entity(), GLboolean _bIsCollided = GL_FALSE, Direction _normalDirection = Direction::UP, glm::vec2 _collisionDir = glm::vec2(0))
		: a(a), b(b), bIsCollided(_bIsCollided), normalDirection(_normalDirection), collisionDir(_collisionDir) {}
	Entity a, b;

	GLboolean bIsCollided;
	Direction normalDirection;
	glm::vec2 collisionDir;

};

struct DestroyEvent : public Event
{
	DestroyEvent() {}
};

struct PlayerDiedEvent : public Event
{
	PlayerDiedEvent() {}
};

struct LevelEnd : public Event
{
	LevelEnd() {}
};
