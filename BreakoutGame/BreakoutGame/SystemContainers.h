#pragma once
#include "BreakoutWorld.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Texture2D.h"
#include "Event.h"

//=================///Components//============

//===== Transform Component ======//

struct TransformComponent
{
	TransformComponent(glm::vec2 _position = {0,0}, GLfloat _rotation = 0 )
		: position(_position), rotation (_rotation) {}

	glm::vec2 position;
	GLfloat rotation;
};

//===== Movement Component ======//

struct MovementComponent
{
	MovementComponent(glm::vec2 _velocity = { 0,0 }, GLfloat _speed = 0)
		: velocity(_velocity), speed(_speed) {}

	glm::vec2 velocity;
	GLfloat speed;
};

//===== Collision Component ======//

enum CollisionShape
{
	Box,
	Circle
};

struct CollisionComponent
{
	CollisionComponent(glm::vec2 _size = { 0,0 }, CollisionShape _shape = Box)
		:shape(_shape), size(_size), radius((_size.x + _size.y)/2){}

	CollisionComponent(GLfloat _radius, CollisionShape _shape = Circle)
		:shape(_shape), radius(_radius), size(_radius) {}

	CollisionShape shape;
	glm::vec2 size;
	GLfloat radius;
};

//===== Rendering Component ======//

struct RenderingComponent
{
	RenderingComponent(glm::vec3 _color = { 1,1,1 }, Texture2D* _texture = nullptr, glm::vec2 _size = { 0,0 })
		: color(_color), texture(_texture), size(_size) {}

	glm::vec3 color;
	Texture2D* texture;
	glm::vec2 size;
};

//===== Input Component ======//

struct InputComponent
{
	InputComponent(){}
};

//===== Destructible Component ======//

struct DestructibleComponent
{
	DestructibleComponent(){}
};

//===== Border Bounce Component ======//

struct BorderBounceComponent
{
	BorderBounceComponent(){}
};

//=================///Events//============

struct InputEvent
{
	InputEvent(InputKey _input = KeyUp)
		:input(_input) {}

	InputKey input;
};

struct CollisionEvent
{
	CollisionEvent(Entity a = Entity(), Entity b = Entity())
		: a(a), b(b) {}
	Entity a, b;

};

//---------TEST------------//

struct InputEvent_H : public Event_H
{
	InputEvent_H(InputKey _input = KeyUp)
		:input(_input) {}

	InputKey input;
};


//=================///System//============

//===== Rendering System ======//

class RenderingSystem : public System
{
public:
	RenderingSystem();
	virtual void Render();
};

//===== Movement System ======//

class MovementSystem : public System
{
public:
	MovementSystem();
	virtual void Update(float dt);
};

//===== Input System ======//

class InputMovementSystem : public System
{
public:
	InputMovementSystem();

	virtual void Init();

	virtual void Update(float dt);


	void OnInputEvent(InputEvent_H* event);
};



//===== Border Bounce System ======//

class BorderBounceSystem : public System
{
public:
	BorderBounceSystem();
	virtual void Update(float dt);
};

//===== Damage System ======//

class DamageSystem : public System
{
public:
	DamageSystem();
	virtual void Update(float dt);
};

//===========//

class SystemContainers
{
public:
	SystemContainers();
	~SystemContainers();
};


