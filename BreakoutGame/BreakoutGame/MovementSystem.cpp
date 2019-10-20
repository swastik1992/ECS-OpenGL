#include "MovementSystem.h"



MovementSystem::MovementSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<MovementComponent>();
}

void MovementSystem::Init()
{
	getWorld().getEventBus().Subscribe(this, &MovementSystem::OnCollisionEvent);
}

void MovementSystem::Update(float dt)
{
	for (auto e : getEntities())
	{
		auto &transformComp = e.getComponent<TransformComponent>();

		if (e.hasComponent<ParentEntityComponent>())
		{
			auto parentComp = e.getComponent< ParentEntityComponent>();
			if (parentComp.parent.hasComponent<MovementComponent>())
			{
				//@TODO 
				auto transformParentComp = parentComp.parent.getComponent<TransformComponent>();
				auto collisionParentComp = parentComp.parent.getComponent<CollisionComponent>();

				auto collisionComp = e.getComponent<CollisionComponent>();

				transformComp.position.x = transformParentComp.position.x + collisionParentComp.size.x / 2 - collisionComp.radius;
				continue;
			}
		}
		auto movementComp = e.getComponent<MovementComponent>();

		transformComp.position.x += movementComp.velocity.x *dt;
		transformComp.position.y += movementComp.velocity.y *dt;

		if (e.hasTag("ball"))
		{
			if (transformComp.position.y >= getWorld().height)
			{
				getWorld().getEventBus().Publish<PlayerDiedEvent>(new PlayerDiedEvent());
			}
		}

		if (e.hasTag("player"))
		{
			if (transformComp.position.x < 0)
			{
				transformComp.position.x = 0;
			}
			else if (transformComp.position.x + 150 > getWorld().width)
			{
				transformComp.position.x = getWorld().width - 150;
			}
		}
	}
}

void MovementSystem::OnCollisionEvent(CollisionEvent* event)
{
	auto collisionCompA = event->a.getComponent<CollisionComponent>();
	auto collisionCompB = event->b.getComponent<CollisionComponent>();

	if (collisionCompA.shape == Circle || collisionCompB.shape == Circle)
	{
		if (event->a.hasTag("player") || event->a.hasTag("border"))
		{
			HandlePlayerCollision(event->b, event->a);
		}
		else if (event->b.hasTag("player") || event->b.hasTag("border"))
		{
			HandlePlayerCollision(event->a, event->b);
		}
		else
			HandleCollisionCircleAndBox(event->a, std::make_tuple(event->bIsCollided, event->normalDirection, event->collisionDir));
	}
	else if (collisionCompA.shape == Box && collisionCompB.shape == Circle)
	{
		if (event->a.hasTag("player") || event->a.hasTag("border"))
		{
			HandlePlayerCollision(event->b, event->a);
		}
		else if (event->b.hasTag("player") || event->b.hasTag("border"))
		{
			HandlePlayerCollision(event->a, event->b);
		}
		else
			HandleCollisionCircleAndBox(event->b, std::make_tuple(event->bIsCollided, event->normalDirection, event->collisionDir));
	}
}

void MovementSystem::HandlePlayerCollision(Entity& circle, Entity& player)
{
	auto& movementCompCircle = circle.getComponent<MovementComponent>();
	auto& transformCompCircle = circle.getComponent<TransformComponent>();
	auto collisionCompCircle = circle.getComponent<CollisionComponent>();

	auto transformCompPlayer = player.getComponent<TransformComponent>();
	auto collisionCompPlayer = player.getComponent<CollisionComponent>();

	GLfloat centerBoard = transformCompPlayer.position.x + collisionCompPlayer.size.x / 2;
	GLfloat distance = (transformCompCircle.position.x + collisionCompCircle.radius) - centerBoard;
	GLfloat percentage = distance / (collisionCompPlayer.size.x / 2);
	// Then move accordingly
	GLfloat strength = 2.0f;
	glm::vec2 oldVelocity = movementCompCircle.velocity;

	movementCompCircle.velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
	//Ball->Velocity.y = -Ball->Velocity.y;
	movementCompCircle.velocity = glm::normalize(movementCompCircle.velocity) * glm::length(oldVelocity); // Keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)

	movementCompCircle.velocity.y = -1 * abs(movementCompCircle.velocity.y);

}

void MovementSystem::HandleCollisionCircleAndBox(Entity& circle, CollisionParam const& collisionParam)
{
	//temp stuff----------
	Direction dir = std::get<1>(collisionParam);
	glm::vec2 diff_vector = std::get<2>(collisionParam);

	//get the movable object.
	auto& movementCompCircle = circle.getComponent<MovementComponent>();
	auto& transformCompCircle = circle.getComponent<TransformComponent>();
	auto collisionCompCircle = circle.getComponent<CollisionComponent>();

	if (dir == LEFT || dir == RIGHT) // Horizontal collision
	{
		movementCompCircle.velocity.x = -movementCompCircle.velocity.x; // Reverse horizontal velocity
		// Relocate
		GLfloat penetration = collisionCompCircle.radius - std::abs(diff_vector.x);
		if (dir == LEFT)
			transformCompCircle.position.x += penetration; // Move ball to right
		else
			transformCompCircle.position.x -= penetration; // Move ball to left;
	}
	else // Vertical collision
	{
		movementCompCircle.velocity.y = -movementCompCircle.velocity.y; // Reverse vertical velocity
		// Relocate
		//GLfloat penetration = collisionCompCircle.radius - std::abs(diff_vector.y);
		//if (dir == UP)
		//	transformCompCircle.position.y -= penetration; // Move ball bback up
		//else
		//	transformCompCircle.position.y += penetration; // Move ball back down
	}
}
