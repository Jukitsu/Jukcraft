#include "pch.h"
#include "entity/BipedEntity.h"
#include "world/World.h"

namespace Jukcraft {
	BipedEntity::BipedEntity(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity
		, float initialYaw, float initialPitch)
		:LivingEntity(world, initialPos, initialVelocity,
			initialYaw, initialPitch, 0.6f, 1.8f) {

	}
	BipedEntity::~BipedEntity() {

	}
	
}