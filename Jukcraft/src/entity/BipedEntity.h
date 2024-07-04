#pragma once
#include "entity/LivingEntity.h"

namespace Jukcraft {

	class BipedEntity : public LivingEntity {
	public:
		BipedEntity(World& world, const glm::vec3& initialPos = glm::vec3(0.0f), 
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			float initialYaw = 0.0f, float initialPitch = 0.0f);
		virtual ~BipedEntity();
	};
}