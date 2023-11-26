#include "pch.h"
#include "entity/LivingEntity.h"
#include "world/World.h"

namespace Jukcraft {
	LivingEntity::LivingEntity(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		const glm::vec3& initialAcceleration, float initialYaw, float initialPitch, float width, float height)
		:Entity(initialPos, initialVelocity, initialAcceleration, initialYaw, initialPitch), oldPosition(position),
		width(0.6f), height(1.8f), collider(), world(world) {
		collider.vx1 = position - glm::vec3(width / 2.0f, 0, width / 2.0f);
		collider.vx2 = position + glm::vec3(width / 2.0f, height, width / 2.0f);
	}
	LivingEntity::~LivingEntity() {

	}
	void LivingEntity::updateCollider() {
		collider.vx1 = position - glm::vec3(width / 2.0f, 0, width / 2.0f);
		collider.vx2 = position + glm::vec3(width / 2.0f, height, width / 2.0f);
	}

	void LivingEntity::resolveCollisions(float deltaTime) {

		glm::vec3 adjustedVelocity = velocity * deltaTime;

		// find all the blocks we could potentially be colliding with
		// this step is known as "broad-phasing"

		int step_x = (int)sign(velocity.x);
		int step_y = (int)sign(velocity.y);
		int step_z = (int)sign(velocity.z);

		int steps_xz = (int)(width / 2);
		int steps_y = (int)(height);

		glm::ivec3 p = static_cast<glm::ivec3>(position);
		glm::ivec3 c = static_cast<glm::ivec3>(position + adjustedVelocity);


		std::vector<CollisionResult> potentialCollisions;
		potentialCollisions.reserve(4 * 4 * 4);

		for (int i = p.x - step_x * (steps_xz + 1); step_x * (i - (c.x + step_x * (steps_xz + 2))) < 0; i += step_x)
			for (int j = p.y - step_y * (steps_y + 2); step_y * (j - (c.y + step_y * (steps_y + 3))) < 0; j += step_y)
				for (int k = p.z - step_z * (steps_xz + 1); step_z * (k - (c.z + step_z * (steps_xz + 2))) < 0; k += step_z) {
					glm::ivec3 pos = { i, j, k };
					BlockID block = world.getBlock(pos);

					if (!block)
						continue;

					for (const Collider& blockCollider : world.getBlocks()[block].getColliders()) {
						CollisionResult result = collider.collide(blockCollider + pos, adjustedVelocity);

						if (result.normal == glm::ivec3(0))
							continue;

						potentialCollisions.push_back(result);
					}

				}


		if (potentialCollisions.empty())
			return;

		auto it = std::min_element(potentialCollisions.begin(), potentialCollisions.end(), [](auto&& a, auto&& b) {
			return a.entryTime < b.entryTime;
			
		});
		auto&& [entryTime, normal] = *it;

		entryTime -= 0.001f;

		if (normal.x) {
			velocity.x = 0;
			position.x += adjustedVelocity.x * entryTime;
		}
		if (normal.y) {
			velocity.y = 0;
			position.y += adjustedVelocity.y * entryTime;
		}
		if (normal.z) {
			velocity.z = 0;
			position.z += adjustedVelocity.z * entryTime;
		}
	}

	void LivingEntity::tick(float deltaTime) {
		oldPosition = position;
		updateCollider();

		for (uint8_t i = 0; i < 3; i++)
			resolveCollisions(deltaTime);

		acceleration += velocity * deltaTime;
		position += velocity * deltaTime;

		velocity = glm::vec3(0.0f);
	}
}