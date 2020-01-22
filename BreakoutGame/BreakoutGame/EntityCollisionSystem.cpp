#include "EntityCollisionSystem.h"

EntityCollisionSystem::EntityCollisionSystem()
{
	requireComponent<TransformComponent>();
	requireComponent<CollisionComponent>();
}

void EntityCollisionSystem::Update(float dt)
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
		for (auto me : movableEntities)
		{
			if (me == e) continue;

			if (e.isAlive() && me.isAlive())
			{
				if ((e.hasComponent<CollisionComponent>()) && (me.hasComponent<CollisionComponent>()))
				{
					auto collisionCompA = e.getComponent<CollisionComponent>();
					auto collisionCompB = me.getComponent<CollisionComponent>();

					if (collisionCompA.shape == Circle || collisionCompB.shape == Circle)
					{
						CollisionParam collision;

						if (collisionCompA.shape == Circle && collisionCompB.shape == Box)
						{
							collision = CheckCollisionCircleAndBox(e, me);
							if (std::get<0>(collision))
							{
								getWorld().getEventBus().Publish(new CollisionEvent(e, me, std::get<0>(collision), std::get<1>(collision), std::get<2>(collision)));
							}
						}
						else if (collisionCompA.shape == Box && collisionCompB.shape == Circle)
						{
							collision = CheckCollisionCircleAndBox(me, e);
							if (std::get<0>(collision))
							{
								getWorld().getEventBus().Publish(new CollisionEvent(me, e, std::get<0>(collision), std::get<1>(collision), std::get<2>(collision)));
							}
						}
					}
					else
					{
						if (CheckCollisionBoxes(e, me) == GL_TRUE)
						{
							getWorld().getEventBus().Publish(new CollisionEvent(e, me, GL_TRUE));
						}
					}

				}
			}
		}

	}
	movableEntities.clear();
}

GLboolean EntityCollisionSystem::CheckCollisionBoxes(Entity& boxA, Entity& boxB) 
{
	auto transformBoxA = boxA.getComponent<TransformComponent>();
	auto collisionBoxA = boxA.getComponent<CollisionComponent>();

	auto transformBoxB = boxB.getComponent<TransformComponent>();
	auto collisionBoxB = boxB.getComponent<CollisionComponent>();

	// Collision x-axis?
	bool collisionX = transformBoxA.position.x + collisionBoxA.size.x >= transformBoxB.position.x &&
		transformBoxB.position.x + collisionBoxB.size.x >= transformBoxA.position.x;
	// Collision y-axis?
	bool collisionY = transformBoxA.position.y + collisionBoxA.size.y >= transformBoxB.position.y &&
		transformBoxB.position.y + collisionBoxB.size.y >= transformBoxA.position.y;
	// Collision only if on both axes
	return collisionX && collisionY;
}

CollisionParam EntityCollisionSystem::CheckCollisionCircleAndBox(Entity& circle, Entity& box)
{
	auto transformCircle = circle.getComponent<TransformComponent>();
	auto collisionCircle = circle.getComponent<CollisionComponent>();

	auto transformBox = box.getComponent<TransformComponent>();
	auto collisionBox = box.getComponent<CollisionComponent>();

	// Get center point circle first
	glm::vec2 center(transformCircle.position + collisionCircle.radius);
	// Calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(collisionBox.size.x / 2, collisionBox.size.y / 2);
	glm::vec2 aabb_center(transformBox.position.x + aabb_half_extents.x, transformBox.position.y + aabb_half_extents.y);
	// Get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// Now that we know the the clamped values, add this to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// Now retrieve vector between center circle and closest point AABB and check if length < radius
	difference = closest - center;

	GLfloat rad = glm::length(difference);

	if ( rad <= collisionCircle.radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

Direction EntityCollisionSystem::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
			glm::vec2(0.0f, 1.0f),	// up
			glm::vec2(1.0f, 0.0f),	// right
			glm::vec2(0.0f, -1.0f),	// down
			glm::vec2(-1.0f, 0.0f)	// left
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

