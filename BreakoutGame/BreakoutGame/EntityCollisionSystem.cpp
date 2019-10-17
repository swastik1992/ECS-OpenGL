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
				auto collisionCompA = e.getComponent<CollisionComponent>();
				auto collisionCompB = me.getComponent<CollisionComponent>();

				if (collisionCompA.shape == Circle || collisionCompB.shape == Circle)
				{
					Collision collision;

					if (collisionCompA.shape == Circle && collisionCompB.shape == Box)
					{
						collision = CheckCollisionCircleAndBox(e, me);
						if (std::get<0>(collision))
						{
							//emit collision event. 
							getWorld().getEventManager().emitEvent<CollisionEvent>(e, me);
							
							//temp stuff
							if (e.hasTag("player"))
							{
								HandlePlayerCollision(me, e);
							}
							else if (me.hasTag("player"))
							{
								HandlePlayerCollision(e, me);
							}
							else
								HandleCollisionCircleAndBox(e,collision);
						}
					}
					else if (collisionCompA.shape == Box && collisionCompB.shape == Circle)
					{
						collision = CheckCollisionCircleAndBox(me, e);
						if (std::get<0>(collision))
						{
							//emit collision event. 
							getWorld().getEventManager().emitEvent<CollisionEvent>(me, e);

							//temp stuff
							if (e.hasTag("player"))
							{
								HandlePlayerCollision(me, e);
							}
							else if (me.hasTag("player"))
							{
								HandlePlayerCollision(e, me);
							}
							else
								HandleCollisionCircleAndBox(me, collision);
						}
					}
				}
				else
				{
					if (CheckCollisionBoxes(e, me) == GL_TRUE)
					{
						//emit collision event. 
						getWorld().getEventManager().emitEvent<CollisionEvent>(e, me);
					}
				}

			}
		}

	}
	movableEntities.clear();
}

void EntityCollisionSystem::HandlePlayerCollision(Entity& circle, Entity& player)
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

void EntityCollisionSystem::HandleCollisionCircleAndBox(Entity& circle, Collision& collisionParam)
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
		//	transformCompCircle.position.y += penetration; // Move ball bback up
		//else
		//	transformCompCircle.position.y -= penetration; // Move ball back down
	}
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

Collision EntityCollisionSystem::CheckCollisionCircleAndBox(Entity& circle, Entity& box)
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

// Calculates which direction a vector is facing (N,E,S or W)
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



//std::tuple<glm::vec2, glm::vec2> CollisionSystem::HitNormalDirection(glm::vec2 direction)
//{
//	std::vector<glm::vec2> directions = { glm::vec2(-1,1), glm::vec2(-1,1), glm::vec2(1,-1), glm::vec2(1,-1) };
//	std::vector<glm::vec2> diffs = { glm::vec2(1,0), glm::vec2(-1,0), glm::vec2(0,-1), glm::vec2(0,1) };
//
//	glm::vec2 compass[] = {
//		glm::vec2(0.0f, 1.0f),	// up
//		glm::vec2(1.0f, 0.0f),	// right
//		glm::vec2(0.0f, -1.0f),	// down
//		glm::vec2(-1.0f, 0.0f)	// left
//	};
//	GLfloat max = 0.0f;
//	GLuint best_match = -1;
//	for (GLuint i = 0; i < 4; i++)
//	{
//		GLfloat dot_product = glm::dot(glm::normalize(direction), compass[i]);
//		if (dot_product > max)
//		{
//			max = dot_product;
//			best_match = i;
//		}
//	}
//	
//	return std::make_tuple(directions[best_match], diffs[best_match]);
//}
//
//CollisionParam CollisionSystem::CheckCollisionCircleAndBox(Entity& circle, Entity& box)
//{
//	auto transformCircle = circle.getComponent<TransformComponent>();
//	auto collisionCircle = circle.getComponent<CollisionComponent>();
//
//	auto transformBox = box.getComponent<TransformComponent>();
//	auto collisionBox = box.getComponent<CollisionComponent>();
//
//	glm::vec2 centerCircle (transformCircle.position + collisionCircle.radius);
//	glm::vec2 centerBox(transformBox.position.x + collisionBox.size.x/2, transformBox.position.y + collisionBox.size.y / 2);
//
//	glm::vec2 difVector(centerBox - centerCircle);
//	glm::vec2 clamped = glm::clamp(difVector, -(collisionBox.size.x / 2, collisionBox.size.y / 2), (collisionBox.size.x / 2, collisionBox.size.y / 2));
//	glm::vec2 closest = centerBox + clamped;
//
//	difVector = closest - centerCircle;
//
//	GLfloat pt = collisionCircle.radius - std::abs(difVector.x);
//	
//	std::tuple<glm::vec2, glm::vec2> tp = HitNormalDirection(difVector);
//	return std::make_tuple((glm::length(difVector) <= collisionCircle.radius), std::get<0>(tp), std::get<1>(tp) * pt);
//}
//
//CollisionParam CollisionSystem::CheckCollisionCircles(Entity& circleA, Entity& circleB)
//{
//	auto transformCompA = circleA.getComponent<TransformComponent>();
//	auto collisionCompA = circleA.getComponent<CollisionComponent>();
//
//	auto transformCompB = circleB.getComponent<TransformComponent>();
//	auto collisionCompB = circleB.getComponent<CollisionComponent>();
//
//	glm::vec2 centerA (transformCompA.position + collisionCompA.radius);
//	glm::vec2 centerB (transformCompB.position + collisionCompB.radius);
//
//	GLfloat distance = glm::distance(centerA, centerB);
//
//	std::tuple<glm::vec2, glm::vec2> tp = HitNormalDirection(centerB - centerA);
//	return std::make_tuple((distance <= collisionCompA.radius || distance <= collisionCompB.radius), std::get<0>(tp), std::get<1>(tp));
//}
//
//CollisionParam CollisionSystem::CheckCollisionBoxes(Entity& boxA, Entity& boxB)
//{
//	auto collisionCompA = boxA.getComponent<CollisionComponent>();
//	auto collisionCompB = boxB.getComponent<CollisionComponent>();
//
//	auto transformCompA = boxA.getComponent<TransformComponent>();
//	auto transformCompB = boxB.getComponent<TransformComponent>();
//
//	 Collision x-axis?
//	bool collisionX = transformCompA.position.x + collisionCompA.size.x >= transformCompB.position.x &&
//		transformCompB.position.x + collisionCompB.size.x >= transformCompA.position.x;
//	 Collision y-axis?
//	bool collisionY = transformCompA.position.y + collisionCompA.size.y >= transformCompB.position.y &&
//		transformCompB.position.y + collisionCompB.size.y >= transformCompA.position.y;
//	 Collision only if on both axes
//
//	std::tuple<glm::vec2, glm::vec2> tp = HitNormalDirection( transformCompB.position - transformCompA.position );
//	return std::make_tuple((collisionX && collisionY), std::get<0>(tp), std::get<1>(tp));
//}
//
//CollisionParam CollisionSystem::CheckCollision(Entity& a, Entity& b)
//{
//	auto collisionCompA = a.getComponent<CollisionComponent>();
//	auto collisionCompB  = b.getComponent<CollisionComponent>();
//
//	if (collisionCompA.shape == Circle || collisionCompB.shape == Circle)
//	{
//		if (collisionCompA.shape == Circle && collisionCompB.shape == Box)
//		{
//			return CheckCollisionCircleAndBox(a, b);
//		}
//		else if (collisionCompA.shape == Box && collisionCompB.shape == Circle)
//		{
//			return CheckCollisionCircleAndBox(b, a);
//		}
//		else
//		{
//			return CheckCollisionCircles(a, b);
//		}
//	}
//	else
//	{
//		return CheckCollisionBoxes(a, b);
//	}
//}