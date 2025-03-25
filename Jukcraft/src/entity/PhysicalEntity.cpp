#include "pch.h"
#include "physics/constants.h"
#include "physics/Collider.h"
#include "world/World.h"
#include "entity/PhysicalEntity.h"

namespace Jukcraft {

	PhysicalEntity::PhysicalEntity(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		float initialYaw, float initialPitch, float width, float height)
		:Entity(initialPos, initialVelocity, initialYaw, initialPitch, width * width * height),
		width(0.6f), height(1.8f), world(world), onGround(false) {
		collider.vx1 = position - glm::vec3(width / 2.0f, 0, width / 2.0f);
		collider.vx2 = position + glm::vec3(width / 2.0f, height, width / 2.0f);
	}
	PhysicalEntity::~PhysicalEntity() {

	}

	

	void PhysicalEntity::applyPhysics() {
		velocity += accel;
		accel = g; 

		resolveCollisions();

		position += velocity;

		velocity *= glm::max(glm::vec3(0.0f), glm::vec3(1.0f) - getFriction());
	}

	void PhysicalEntity::move(const glm::vec3& motion) {
		velocity += motion;
	}

	void PhysicalEntity::push(const glm::vec3& force) {
		accel += force / mass; // Newton's second law
		hasImpulse = true;
	}

	void PhysicalEntity::resolveCollisions() {
		updateCollider();

		onGround = false;
		onWall = false;

		for (uint8_t _ = 0; _ < 6; _++) {
			glm::vec3 adjustedVelocity = velocity;

			// find all the blocks we could potentially be colliding with
			// this step is known as "broad-phasing"

			int step_x = sign(velocity.x);
			int step_y = sign(velocity.y);
			int step_z = sign(velocity.z);

			int steps_xz = static_cast<int>(width / 2);
			int steps_y = static_cast<int>(height);

			glm::ivec3 p = static_cast<glm::ivec3>(position);
			glm::ivec3 c = static_cast<glm::ivec3>(position + adjustedVelocity);


			std::vector<CollisionResult> potentialCollisions;
			potentialCollisions.reserve(4 * 4 * 4);

			for (int32_t i = p.x - step_x * (steps_xz + 1); step_x * (i - (c.x + step_x * (steps_xz + 2))) < 0; i += step_x)
				for (int32_t j = p.y - step_y * (steps_y + 2); step_y * (j - (c.y + step_y * (steps_y + 3))) < 0; j += step_y)
					for (int32_t k = p.z - step_z * (steps_xz + 1); step_z * (k - (c.z + step_z * (steps_xz + 2))) < 0; k += step_z) {
						BlockPos pos(i, j, k);
						BlockID block = world.getBlock(pos);
						std::vector<Entity*> entities = world.getEntitiesAt(glm::ivec3(i, j, k));

						if (!block && entities.empty())
							continue;

						for (const Collider& blockCollider : world.getBlocks()[block].getColliders()) {
							CollisionResult result = collider.collide(blockCollider + pos, adjustedVelocity);

							if (result.normal == glm::ivec3(0))
								continue;

							potentialCollisions.emplace_back(std::move(result));
						}

						for (auto&& entity : entities) {
							if (entity != this && entity->isPhysical()) {
								const Collider& entityCollider = entity->getCollider();

								CollisionResult result = collider.collide(entityCollider, adjustedVelocity);

								if (result.normal == glm::ivec3(0))
									continue;

								result.source = entity;

								potentialCollisions.emplace_back(std::move(result));
							}
						}
					}


			if (potentialCollisions.empty())
				return;

			auto it = std::min_element(potentialCollisions.begin(), potentialCollisions.end(), [](auto&& a, auto&& b) {
				return a.entryTime < b.entryTime;

				});
			auto&& [entryTime, normal, source] = *it;

			entryTime -= 0.01f;

			if (normal.x) {
				onWall = true;
				velocity.x = 0;
				position.x += adjustedVelocity.x * entryTime;

			}
			if (normal.y) {
				velocity.y = 0;
				position.y += adjustedVelocity.y * entryTime;
			}
			if (normal.z) {
				onWall = true;
				velocity.z = 0;
				position.z += adjustedVelocity.z * entryTime;

			}
			if (normal.y == 1) {
				onGround = true;
				hasImpulse = false;
			}

			if (source) {
				float mass2 = source->getMass();
				glm::vec3 velocity2 = source->getVelocity();
			
				this->setVelocity((mass - mass2) / (mass + mass2) * velocity 
						      + 2 * mass2 / (mass + mass2) * velocity2);
				source->setVelocity((mass - mass2) / (mass + mass2) * velocity
					+ 2 * mass2 / (mass + mass2) * velocity2);

			}


			hasCollision = onGround || onWall;

		}
		

		
	}
}