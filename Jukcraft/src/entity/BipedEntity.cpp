#include "pch.h"
#include "entity/BipedEntity.h"
#include "world/World.h"

namespace Jukcraft {
	BipedEntity::BipedEntity(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		const glm::vec3& initialAcceleration, float initialYaw, float initialPitch)
		:LivingEntity(world, initialPos, initialVelocity, 
			initialAcceleration, initialYaw, initialPitch, 0.6f, 1.8f) {

	}
	BipedEntity::~BipedEntity() {

	}
	
}