#include "pch.h"
#include "entity/player/Player.h"
#include "world/World.h"

namespace Jukcraft {
	Player::Player(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		const glm::vec3& initialAcceleration, float initialYaw, float initialPitch)
		:BipedEntity(world, initialPos, initialVelocity, initialAcceleration, initialYaw, initialPitch) {


	}
	Player::~Player() {

	}


}