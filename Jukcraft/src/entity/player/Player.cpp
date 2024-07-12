#include "pch.h"
#include "entity/player/Player.h"
#include "world/World.h"

namespace Jukcraft {
	Player::Player(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
			float initialYaw, float initialPitch)
		:BipedEntity(world, initialPos, initialVelocity, initialYaw, initialPitch) {


	}
	Player::~Player() {

	}

	void Player::aiStep() {
		BipedEntity::aiStep();
		dashCooldown = glm::max(dashCooldown - 1, 0);
		if (dashing && onGround)
			dashing = false;
	}

	void Player::dash() {
		if (!dashing && dashCooldown <= 0) {
			velocity += glm::vec3(
				glm::cos(yaw) * glm::cos(pitch),
				glm::sin(pitch),
				glm::sin(yaw) * glm::cos(pitch)
			) * DASH_SPEED;
			dashing = true;
			dashCooldown = (int16_t)TICK_RATE;
		}
		
	}


}