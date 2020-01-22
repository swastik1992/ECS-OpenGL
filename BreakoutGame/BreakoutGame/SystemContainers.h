#pragma once
#include "BreakoutWorld.h"
#include "Texture2D.h"
#include "Event.h"
#include "EntityCollisionSystem.h"
#include "Utils.h"
#include "GameEvents.h"
#include "GameComponents.h"

//To keep all the small systems game will be using. 
////////////////////////////////////////////////////////

//=================///System//============

//===== Rendering System ======//

class RenderingSystem : public System
{
public:
	RenderingSystem();
	virtual void Render();
};


//===== Input System ======//

class InputMovementSystem : public System
{
public:
	InputMovementSystem();
	virtual void Init();
	void OnInputEvent(InputEvent* event);
};


//===== Post Processing System ======//

class PostProcessingSystem : public System
{
public:
	PostProcessingSystem() {}
	virtual void Init();
	virtual void Update(float dt);
	void OnDestroyEvent(DestroyEvent* event);
private:
	PostProcessing* pProcessing;
	GLfloat shakeTime;
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
	DamageSystem() {}
	virtual void Init();
	void OnCollisionEvent(CollisionEvent* event);
};


//===== Damage System ======//

class GamsStateSystem : public System
{
public:
	GamsStateSystem();
	virtual void Update(float dt);
};

//===========//

class SystemContainers
{
public:
	SystemContainers();
	~SystemContainers();
};


