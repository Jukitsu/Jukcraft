#include "pch.h"
#include "entity/player/Player.h"

namespace Jukcraft {
	Player::Player(const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		const glm::vec3& initialAcceleration, float initialYaw, float initialPitch)
		:Mob(initialPos, initialVelocity, initialAcceleration, initialYaw, initialPitch) {


	}
	Player::~Player() {

	}

}