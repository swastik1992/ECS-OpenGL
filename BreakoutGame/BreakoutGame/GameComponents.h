#pragma once
#include "Utils.h"
#include "Entity.h"
//=================///Components//============

//===== Transform Component ======//

struct TransformComponent
{
	TransformComponent(glm::vec2 _position = { 0,0 }, GLfloat _rotation = 0)
		: position(_position), rotation(_rotation) {}

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

//===== Parent Entity Component ======//

struct ParentEntityComponent
{
	ParentEntityComponent() {}
	ParentEntityComponent(Entity _parent)
		: parent(_parent) {}

	Entity parent;
};

//===== Collision Component ======//



struct CollisionComponent
{
	CollisionComponent(glm::vec2 _size = { 0,0 }, CollisionShape _shape = Box)
		:shape(_shape), size(_size), radius((_size.x + _size.y) / 2) {}

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
	InputComponent() {}
};

//===== Destructible Component ======//

struct DestructibleComponent
{
	DestructibleComponent() {}
};

//===== Border Bounce Component ======//

struct BorderBounceComponent
{
	BorderBounceComponent() {}
};
