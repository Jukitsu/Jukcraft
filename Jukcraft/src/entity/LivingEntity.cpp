#include "pch.h"
#include "entity/LivingEntity.h"
#include "world/World.h"

#include "physics/constants.h"

#include "core/App.h"


namespace Jukcraft {

	LivingEntity::LivingEntity(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		float initialYaw, float initialPitch, float width, float height)
		:PhysicalEntity(world, initialPos, initialVelocity, initialYaw, initialPitch, width, height), 
		speed(WALK_SPEED), input(0.0f),
		bodyRot(rotation), headRot(rotation) {

		updateOldState();
	}
	LivingEntity::~LivingEntity() {

	}

	void LivingEntity::updateCollider() {
		collider.vx1 = position - glm::vec3(width / 2.0f, 0, width / 2.0f);
		collider.vx2 = position + glm::vec3(width / 2.0f, height, width / 2.0f);
	}

	void LivingEntity::updateOldState() {
		old.position = position;
		old.bodyRot = bodyRot;
		old.headRot = headRot;
		old.velocity = velocity;
	}

	void LivingEntity::tick() {
		updateOldState();

		age++;
		iframes = glm::max(0, iframes - 1);

		aiStep();

		if (deathTime <= TICK_RATE) {
			tickRotations();
			animate();
		}
		else {
			walkAnimation.freeze();
		}

		if (health.hp <= 0.0f || deathTime > 0) {
			deathTime++;
		}

		setInput(glm::vec3(0.0f));
	}

	void LivingEntity::aiStep() {
		if (deathTime <= 0)
			tickAi();
		applyPhysics();
		checkInjury();
		health.healNatural();
		stamina = glm::min(stamina + 0.5f / TICK_RATE, 1.0f);
	}

	void LivingEntity::applyPhysics() {
		if (input.x || input.z && stamina > -1.0f) {
			float headingAngle = rotation.x - glm::atan<float>(input.z, input.x) + glm::pi<float>() / 2;

			velocity += glm::vec3(
				glm::cos(headingAngle) * speed,
				input.y,
				glm::sin(headingAngle) * speed
			) * getFriction();
		}

		if (jumping)
			jump();

		PhysicalEntity::applyPhysics();

	}

	void LivingEntity::setJumping(bool state) {
		jumping = state;
	}

	void LivingEntity::jump(float hmax) {
		jumping = false;

		if (!onGround || stamina < -0.5f)
			return;

		velocity.y = glm::sqrt(-2 * g.y * hmax);
	}


	void LivingEntity::checkInjury() {
		if (iframes <= 0) {
			float unabsorbedEnergy = absorbedEnergy - getMaxKineticEnergy();
			if (unabsorbedEnergy > 0.0f) {
				hurt(unabsorbedEnergy * TICK_RATE * TICK_RATE / 5.0f, glm::vec3(0.0f));
			}
		}
	}


	void LivingEntity::hurt(float amount, const glm::vec3& knockback) {
		bool success = health.hurt(amount, iframes > 0);
		if (success)
			push(mass * knockback / (iframes > 0 ? 2.0f : 1.0f));
		iframes = iframes > 0 ? iframes : glm::floor(TICK_RATE / 2);
		if (health.hp <= 0.0f) {
			die();
		}
	}
	
	void LivingEntity::tickRotations() {
		glm::vec3 ds = position - old.position;
		float nextBodyYaw = headRot.x;
		if (ds.x * ds.x + ds.z * ds.z > 0.0f) {
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

	void LivingEntity::animate() {
		glm::vec3 vector = hasImpulse ? position - old.position
			: glm::vec3(position.x - old.position.x, 0.0f, position.z - old.position.z);
			
		walkAnimation.update(glm::min(glm::length(vector) * 4.0f,
			1.0f),  
			0.4f);
	}

	void LivingEntity::die() {
		deathTime++;
		setInput(glm::vec3(0.0f));
	}
	

}