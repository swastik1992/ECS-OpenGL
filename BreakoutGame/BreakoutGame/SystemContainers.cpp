#include "SystemContainers.h"
#include "AssetLoader.h"
SystemContainers::SystemContainers()
{
}

SystemContainers::~SystemContainers()
{
}

//===== RenderingSystem System ======//

RenderingSystem::RenderingSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<RenderingComponent>();
}

void RenderingSystem::Render()
{
	for (auto e : getEntities())
	{
		if ((e.hasComponent<RenderingComponent>()) && (e.hasComponent<TransformComponent>()))
		{
			auto renderingComp = e.getComponent<RenderingComponent>();
			auto transformComp = e.getComponent<TransformComponent>();

			BreakoutWorld* world = dynamic_cast<BreakoutWorld*>(&getWorld());
			world->GetSpriteRenderer()->DrawSprite(renderingComp.texture, transformComp.position, renderingComp.size, transformComp.rotation, renderingComp.color);
		}
	}
}

//===== InputMovementSystem System ======//

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

void InputMovementSystem::OnInputEvent(InputEvent* event)
{
	if (event->inputAction == Down)
	{
		if (event->key == GLFW_KEY_A || event->key == GLFW_KEY_LEFT)
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
		else if (event->key == GLFW_KEY_D|| event->key == GLFW_KEY_RIGHT)
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
		
	}
	else
	{
		for (auto e : getEntities())
		{
			auto &movementComp = e.getComponent<MovementComponent>();
			movementComp.velocity.x = 0;
		}
		
	}

}

//===== BorderBounceSystem System ======//

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
		if (!e.hasComponent<TransformComponent>()) continue;
		if (!e.hasComponent<MovementComponent>()) continue;
		if (!e.hasComponent<CollisionComponent>()) continue;

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

//====== Damage System =====//

void DamageSystem::Init()
{
	getWorld().getEventBus().Subscribe(this, &DamageSystem::OnCollisionEvent);
}

void DamageSystem::OnCollisionEvent(CollisionEvent* event)
{
	if (BreakoutWorld* world = static_cast<BreakoutWorld*>(&getWorld()))
	{
		GLboolean bIsDestroyed = GL_FALSE;
		if (getWorld().getEntityManager().hasComponent<DestructibleComponent>(event->a))
		{
			getWorld().getEventBus().Publish<DestroyEvent>(new DestroyEvent());
			event->a.kill();
			bIsDestroyed = GL_TRUE;
		}

		if (getWorld().getEntityManager().hasComponent<DestructibleComponent>(event->b))
		{
			getWorld().getEventBus().Publish<DestroyEvent>(new DestroyEvent());
			event->b.kill();
			bIsDestroyed = GL_TRUE;
		}

		if (world->gameState == GameState::InGame)
		{
			if (bIsDestroyed)
				world->GetSoundEngine()->play2D("Resource/audio/446100__justinvoke__bounce.wav", false);
			else
				world->GetSoundEngine()->play2D("Resource/audio/solid.wav", false);
		}
	}

}

//===== PostProcessingSystem System ======//

void PostProcessingSystem::Init()
{
	BreakoutWorld* bWorld = static_cast<BreakoutWorld*>(&getWorld());
	if (!bWorld) return;
	pProcessing = bWorld->GetPostProcessing();
	shakeTime = 0;

	getWorld().getEventBus().Subscribe(this, &PostProcessingSystem::OnDestroyEvent);
}

void PostProcessingSystem::Update(float dt)
{
	if (!pProcessing) return;

	if (shakeTime > 0)
		shakeTime -= dt;
	else
		pProcessing->Shake = GL_FALSE;

}

void PostProcessingSystem::OnDestroyEvent(DestroyEvent* event)
{
	if(!pProcessing) return;
	
	pProcessing->Shake = GL_TRUE;
	shakeTime = 0.05f;
}

//===== GamsStateSystem System ======//

GamsStateSystem::GamsStateSystem()
{
	requireComponent<DestructibleComponent>();
}

void GamsStateSystem::Update(float dt)
{
	if (getEntities().size() <= 0)
	{
		if (BreakoutWorld* world = static_cast<BreakoutWorld*>(&getWorld()))
		{
			if (world->gameState == GameState::InGame)
				world->getEventBus().Publish< LevelEnd>(new LevelEnd());
		}
	}
}


