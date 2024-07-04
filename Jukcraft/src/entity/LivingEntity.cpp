#include "pch.h"
#include "entity/LivingEntity.h"
#include "world/World.h"

#include "physics/constants.h"

namespace Jukcraft {

	LivingEntity::LivingEntity(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		float initialYaw, float initialPitch, float width, float height)
		:Entity(initialPos, initialVelocity, initialYaw, initialPitch, width * width * height),
		oldPosition(position), interpolatedPos(position), interpolationStep(1.0f), input(0),
		width(width), height(height), collider(), world(world), onGround(false), speed(WALK_SPEED) {
		LOG_TRACE("{}", velocity.y);
		collider.vx1 = position - glm::vec3(width / 2.0f, 0, width / 2.0f);
		collider.vx2 = position + glm::vec3(width / 2.0f, height, width / 2.0f);
	}
	LivingEntity::~LivingEntity() {

	}

	void LivingEntity::updateCollider() {
		collider.vx1 = position - glm::vec3(width / 2.0f, 0, width / 2.0f);
		collider.vx2 = position + glm::vec3(width / 2.0f, height, width / 2.0f);
	}

	void LivingEntity::resolveCollisions() {

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
		if (normal.y == 1) {
			onGround = true;
		}

	}

	void LivingEntity::jump(float hmax) {
		if (!onGround)
			return;

		velocity.y = glm::sqrt(-2 * g.y * hmax);
	}

	void LivingEntity::tick() {
		aiStep();
	}

	void LivingEntity::aiStep() {
		applyPhysics();
	}

	void LivingEntity::applyPhysics() {
		if (input.x || input.z) {
			float headingAngle = yaw - glm::atan<float>((float)input.z, (float)input.x) + glm::pi<float>() / 2;

			velocity += glm::vec3(
				glm::cos(headingAngle) * speed,
				input.y,
				glm::sin(headingAngle) * speed
			) * getFriction();

			setInput(glm::ivec3(0));
		}


		updateCollider();

		onGround = false;

		for (uint8_t i = 0; i < 3; i++)
			resolveCollisions();

		oldPosition = position;

		position += velocity;
		velocity += g;
		velocity *= glm::max(glm::vec3(0.0), glm::vec3(1.0) - getFriction());

		updateCollider();
	}

	void LivingEntity::move(const glm::vec3& motion) {
		velocity += motion;
	}

	void LivingEntity::push(const glm::vec3& motion) {
		velocity += motion;
	}
}