#include "pch.h"
#include "entity/player/Player.h"
#include "world/World.h"

namespace Jukcraft {
	Player::Player(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
			float initialYaw, float initialPitch)
		:LivingEntity(world, initialPos, initialVelocity, initialYaw, initialPitch, 0.6f, 1.8f) {


	}
	Player::~Player() {

	}

	void Player::aiStep() {
		fovOld = fov;
		LivingEntity::aiStep();
		headRot = rotation;
		dashCooldown = glm::max(dashCooldown - 1, 0);
		if (dashing && onGround) {
			fov = glm::mix(fov, glm::radians(70.0f), 2 / TICK_RATE);
			dashing = false;
		}
			
	}

	void Player::dash() {
		if (!dashing && dashCooldown <= 0) {
			velocity += glm::vec3(
				glm::cos(rotation.x) * glm::cos(rotation.y),
				glm::sin(rotation.y),
				glm::sin(rotation.x) * glm::cos(rotation.y)
			) * DASH_SPEED;
			fov = glm::mix(glm::radians(70.0f), glm::radians(80.0f), 2 / TICK_RATE);
			dashing = true;
			dashCooldown = (int16_t)TICK_RATE / 4.0F;
		}
	}


}