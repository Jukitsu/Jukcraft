#include "pch.h"
#include "entity/LivingEntity.h"
#include "world/World.h"

#include "physics/constants.h"



namespace Jukcraft {

	LivingEntity::LivingEntity(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		float initialYaw, float initialPitch, float width, float height)
		:Entity(initialPos, initialVelocity, initialYaw, initialPitch, width * width * height), 
		width(0.6f), height(1.8f), collider(), world(world), onGround(false), speed(WALK_SPEED), input(0.0f),
		bodyRot(rotation), headRot(rotation) {

		old.position = position;
		old.bodyRot = bodyRot;
		old.headRot = headRot;
		old.velocity = velocity;
		

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
		if (!onGround || stamina < -0.5f)
			return;

		if (input.z > 0.0f && stamina > 0.0f) {
			velocity.x += glm::cos(rotation.x) * glm::length(inertia);
			velocity.z += glm::sin(rotation.x) * glm::length(inertia);
			consumeInertia();
		}

		velocity.y = glm::sqrt(-2 * g.y * hmax);
	}

	void LivingEntity::tickRotations() {
		glm::vec3 ds = position - old.position;
		float nextBodyYaw = headRot.x;
		float ortho2 = ds.x * ds.x + ds.z * ds.z;
		if (ortho2 > 0.0f) {
			nextBodyYaw = glm::atan<float>(ds.z, ds.x);
		}
		if (animationTicks.attack > 0.0F) {
			nextBodyYaw = getYaw();
		}

		setBodyYaw(bodyRot.x + wrapRadians(nextBodyYaw - bodyRot.x) * 0.3f);
		
		float relativeAngle = wrapRadians(headRot.x - bodyRot.x);
		
		if (glm::abs(relativeAngle) > glm::pi<float>() / 3.0f) {
			bodyRot.x += relativeAngle - glm::sign(relativeAngle) * glm::pi<float>() / 3.0f;
		}

		while (bodyRot.x - old.bodyRot.x < -glm::pi<float>()) {
			old.bodyRot.x -= 2.0f * glm::pi<float>();
		}

		while (bodyRot.x - old.bodyRot.x >= glm::pi<float>()) {
			old.bodyRot.x += 2.0f * glm::pi<float>();
		}

		while (headRot.x - old.headRot.x < -glm::pi<float>()) {
			old.headRot.x -= 2.0f * glm::pi<float>();
		}

		while (headRot.x - old.headRot.x >= glm::pi<float>()) {
			old.headRot.x += 2.0f * glm::pi<float>();
		}
	}

	void LivingEntity::tick() {
		old.position = position;
		old.bodyRot = bodyRot;
		old.headRot = headRot;
		old.velocity = velocity;

		age++;
		iframes = glm::max(0, iframes - 1);
	
		aiStep();
		tickRotations();

		walkAnimation.update(glm::min(glm::length(position - old.position) * 4.0f,
			1.0f), //TICK_RATE / 20.0f), 
			0.4f);

		if (health.hp <= 0.0f || deathTime > 0) {
			die();
		}
	}

	void LivingEntity::die() {
		deathTime++;
		setInput(glm::vec3(0.0f));
	}

	void LivingEntity::hurt(float amount, const glm::vec3& knockback) {
		bool success = health.hurt(amount, iframes > 0);
		velocity += (float)(success) * knockback / (iframes > 0 ? 2.0f : 1.0f);
		iframes = iframes > 0 ? iframes : glm::floor(TICK_RATE / 2);
		if (health.hp <= 0.0f) {
			die();
		}
	}

	void LivingEntity::aiStep() {
		applyPhysics();
		checkInjury();
		health.healNatural();
		stamina = glm::min(stamina + 1.0f / TICK_RATE, 1.0f);
	}

	void LivingEntity::checkInjury() {
		float unabsorbedEnergy = getKineticEnergy() - getMaxKineticEnergy();
		if (unabsorbedEnergy > 0.0f) {
			hurt(health.hp - unabsorbedEnergy * TICK_RATE * TICK_RATE, glm::vec3(0.0f));
			consumeInertia();
		}
	}

	void LivingEntity::applyPhysics() {
		if (input.x || input.z) {
			float headingAngle = rotation.x - glm::atan<float>(input.z, input.x) + glm::pi<float>() / 2;

			velocity += glm::vec3(
				glm::cos(headingAngle) * speed,
				input.y,
				glm::sin(headingAngle) * speed
			) * getFriction();

			setInput(glm::vec3(0.0f));
		}

		updateCollider();

		onGround = false;

		for (uint8_t i = 0; i < 3; i++)
			resolveCollisions();

		position += velocity;
		velocity += g;
		velocity *= glm::max(glm::vec3(0.0f), glm::vec3(1.0f) - getFriction());

		inertia += old.velocity - velocity;

		inertia *= glm::vec3(1.0f) - DRAG_FLY;

		updateCollider();
	}

	void LivingEntity::move(const glm::vec3& motion) {
		velocity += motion;
	}

	void LivingEntity::push(const glm::vec3& motion) {
		velocity += motion;
	}

	void LivingEntity::consumeInertia() {
		stamina = glm::max(-1.0f, stamina - 2 * getKineticEnergy() * TICK_RATE * TICK_RATE);
		inertia = glm::vec3(0.0f);
	}
}