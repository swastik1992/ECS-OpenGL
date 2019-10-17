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

void InputMovementSystem::Init()
{
	getWorld().getEventBus().Subscribe(this, &InputMovementSystem::OnInputEvent);
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

void InputMovementSystem::OnInputEvent(InputEvent_H* event)
{
	if (event->input == KeyUp)
		std::cout << "Key is up." << std::endl;
	else if(event->input == KeyDown)
		std::cout << "Key is down." << std::endl;

}

//===========//



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
	//auto collisionEvents = getWorld().getEventManager().getEvents<CollisionEvent>();
	//for (auto event : collisionEvents)
	//{
	//	if (event.b.IsValid())
	//	{
	//		//std::cout << "yoyo" << getWorld().getEntityManager().tag event. . toString() << std::endl;

	//		if (getWorld().getEntityManager().hasComponent<BorderBounceComponent>(event.b))
	//		{
	//			if (getWorld().getEntityManager().hasComponent<MovementComponent>(event.b))
	//			{
	//				auto &transformComp = event.b.getComponent<TransformComponent>();
	//				auto &movementComp = event.b.getComponent<MovementComponent>();
	//				movementComp.velocity *= -event.hitNormal;
	//				//transformComp.position = event.diff;
	//			}

	//		}
	//	}
	//};

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
{}

void DamageSystem::Update(float dt)
{
	auto collisionEvents = getWorld().getEventManager().getEvents<CollisionEvent>();
	for (auto e : collisionEvents)
	{
		if (getWorld().getEntityManager().hasComponent<DestructibleComponent>(e.a))
		{
			e.a.kill();
		}

		if (getWorld().getEntityManager().hasComponent<DestructibleComponent>(e.b))
		{
			e.b.kill();
		}
	}
}


