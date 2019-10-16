#include "SystemContainers.h"
#include "AssetLoader.h"


SystemContainers::SystemContainers()
{
}

SystemContainers::~SystemContainers()
{
}

//===========//

RenderingSystem::RenderingSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<RenderingComponent>();
}

void RenderingSystem::Render()
{
	for (auto e : getEntities())
	{
		auto renderingComp = e.getComponent<RenderingComponent>();
		auto transformComp = e.getComponent<TransformComponent>();

		BreakoutWorld* world = dynamic_cast<BreakoutWorld*>(&getWorld());
		world->GetSpriteRenderer()->DrawSprite( renderingComp.texture, transformComp.position, renderingComp.size, transformComp.rotation, renderingComp.color);

	}
}

//===========//

MovementSystem::MovementSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<MovementComponent>();
}

void MovementSystem::Update(float dt)
{
	//auto collisionEvents = getWorld().getEventManager().getEvents<CollisionEvent>();
	/*for (auto event : collisionEvents) 
	{
	};*/

	for (auto e : getEntities())
	{
		auto &transformComp = e.getComponent<TransformComponent>();
		auto movementComp = e.getComponent<MovementComponent>();

		transformComp.position.x += movementComp.velocity.x *dt;
		transformComp.position.y += movementComp.velocity.y *dt;
	}
}

//===========//

InputMovementSystem::InputMovementSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<MovementComponent>();
	requireComponent<CollisionComponent>();
	requireComponent<InputComponent>();
}

void InputMovementSystem::Update(float dt)
{
	auto inputEvents = getWorld().getEventManager().getEvents<InputEvent>();
	for (auto event : inputEvents)
	{
		if (event.input == LeftDown)
		{
			for (auto e : getEntities())
			{
				auto transformComp = e.getComponent<TransformComponent>();
				auto &movementComp = e.getComponent<MovementComponent>();

				if (transformComp.position.x >= 0)
				{
					movementComp.velocity.x = -(movementComp.speed);
				}
			}
		
		}
		else if (event.input == RightDown)
		{
			for (auto e : getEntities())
			{
				auto transformComp = e.getComponent<TransformComponent>();
				auto &movementComp = e.getComponent<MovementComponent>();
				auto collisionComp = e.getComponent<CollisionComponent>();
				BreakoutWorld* world = dynamic_cast<BreakoutWorld*>(&getWorld());

				if (transformComp.position.x <= world->width - collisionComp.size.x)
				{
					movementComp.velocity.x = (movementComp.speed);
				}
			}
		}
		else if(event.input == KeyUp)
		{
			for (auto e : getEntities())
			{
				auto &movementComp = e.getComponent<MovementComponent>();
				movementComp.velocity.x = 0;
			}
		}
	}
}

//===========//

CollisionSystem::CollisionSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<CollisionComponent>();
}

void CollisionSystem::Update(float dt)
{
	std::vector<Entity> movableEntities;

	for (auto e : getEntities())
	{
		if (e.isAlive())
		{
			if (e.hasComponent<MovementComponent>())
				movableEntities.push_back(e);
		}
	}

	for (auto e : getEntities())
	{
		for(auto me : movableEntities)
		{
			if (me == e) continue;

			if (e.isAlive() && me.isAlive())
			{
				if (CheckCollision(e, me) == GL_TRUE)
				{
					getWorld().getEventManager().emitEvent<CollisionEvent>(e, me);
				}
			}
		}
		
	}
	movableEntities.clear();
}

GLboolean CollisionSystem::CheckCollision(Entity& a, Entity& b)
{
	auto transformCompA = a.getComponent<TransformComponent>();
	auto collisionCompA = a.getComponent<CollisionComponent>();
	
	auto transformCompB = b.getComponent<TransformComponent>();
	auto collisionCompB  = b.getComponent<CollisionComponent>();

	// Collision x-axis?
	bool collisionX = transformCompA.position.x + collisionCompA.size.x >= transformCompB.position.x &&
		transformCompB.position.x + collisionCompB.size.x >= transformCompA.position.x;
	// Collision y-axis?
	bool collisionY = transformCompA.position.y + collisionCompA.size.y >= transformCompB.position.y &&
		transformCompB.position.y + collisionCompB.size.y >= transformCompA.position.y;
	// Collision only if on both axes
	return collisionX && collisionY;
}

//===========//

BorderBounceSystem::BorderBounceSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<MovementComponent>();
	requireComponent<BorderBounceComponent>();
	requireComponent<CollisionComponent>();
}

void BorderBounceSystem::Update(float dt)
{
	BreakoutWorld* world = dynamic_cast<BreakoutWorld*>(&getWorld());
	for (auto e : getEntities())
	{
		auto &transformComp = e.getComponent<TransformComponent>();
		auto &movementComp = e.getComponent<MovementComponent>();
		auto collisionComp = e.getComponent<CollisionComponent>();

		if (transformComp.position.x <= 0.0f)
		{
			movementComp.velocity.x = -movementComp.velocity.x;
			transformComp.position.x = 0.0f;
		}
		else if (transformComp.position.x + collisionComp.size.x >= world->width )
		{
			movementComp.velocity.x = -movementComp.velocity.x;
			transformComp.position.x = world->width - collisionComp.size.x;
		}
		if (transformComp.position.y <= 0.0f)
		{
			movementComp.velocity.y = -movementComp.velocity.y;
			transformComp.position.y = 0.0f;
		}

	}
}

//===========//

DamageSystem::DamageSystem()
{
}

void DamageSystem::Update(float dt)
{
	auto collisionEvents = getWorld().getEventManager().getEvents<CollisionEvent>();
	for (auto e : collisionEvents)
	{
		if (getWorld().getEntityManager().hasComponent<DestructibleComponent>(e.a))
		{
			getWorld().getEntityManager().destroyEntity(e.a);
			e.a.kill();
		}

		if (getWorld().getEntityManager().hasComponent<DestructibleComponent>(e.b))
		{
			getWorld().getEntityManager().destroyEntity(e.b);
		}
		//if (e.a.isAlive())
		//{
		//	if (e.a.hasComponent<DestructibleComponent>())
		//	{
		//		//e.a.kill();
		//	}
		//}

		//if (e.b.isAlive())
		//{
		//	if (e.b.hasComponent<DestructibleComponent>())
		//	{
		//		//e.b.kill();
		//	}
		//}
	}
}


