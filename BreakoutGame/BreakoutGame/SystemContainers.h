#pragma once
#include "BreakoutWorld.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Texture2D.h"

//=================///Components//============

//keeping components here temp.
struct TransformComponent
{
	TransformComponent(glm::vec2 _position = {0,0}, GLfloat _rotation = 0 )
		: position(_position), rotation (_rotation)
	{}

	glm::vec2 position;
	GLfloat rotation;
};

struct MovementComponent
{
	MovementComponent(glm::vec2 _velocity = { 0,0 }, GLfloat _speed = 0)
		: velocity(_velocity), speed(_speed)
	{}

	glm::vec2 velocity;
	GLfloat speed;
};

struct CollisionComponent
{
	CollisionComponent(glm::vec2 _size = { 0,0 })
		: size(_size)
	{}

	glm::vec2 size;
};

struct RenderingComponent
{
	RenderingComponent(glm::vec3 _color = { 1,1,1 }, Texture2D* _texture = nullptr, glm::vec2 _size = { 0,0 })
		: color(_color), texture(_texture), size(_size)
	{}

	glm::vec3 color;
	Texture2D* texture;
	glm::vec2 size;
};

struct InputComponent
{
	InputComponent(){}
};

struct DestructibleComponent
{
	DestructibleComponent(){}
};

struct BorderBounceComponent
{
	BorderBounceComponent(){}
};

//=================///System//============

class RenderingSystem : public System
{
public:
	RenderingSystem();
	virtual void Render();
};

class MovementSystem : public System
{
public:
	MovementSystem();
	virtual void Update(float dt);
};


class InputMovementSystem : public System
{
public:
	InputMovementSystem();
	virtual void Update(float dt);
};

class CollisionSystem : public System
{
public:
	CollisionSystem();
	virtual void Update(float dt);
protected:
	GLboolean CheckCollision(Entity& a, Entity& b);

};

class BorderBounceSystem : public System
{
public:
	BorderBounceSystem();
	virtual void Update(float dt);
};

class DamageSystem : public System
{
public:
	DamageSystem();
	virtual void Update(float dt);
};

class SystemContainers
{
public:
	SystemContainers();
	~SystemContainers();
};


//=================///Events//============
enum InputKey
{
	LeftDown,
	RightDown,
	KeyUp
};

struct InputEvent
{
	InputEvent(InputKey _input = KeyUp)
		:input(_input)
	{}

	InputKey input;
};



struct CollisionEvent
{
	CollisionEvent(Entity a = Entity(), Entity b = Entity()) : a(a), b(b) {}
	Entity a, b;
};