#pragma once
#include "entity/LivingEntity.h"

namespace Jukcraft {
	class Mob : public LivingEntity {
	public:
		Mob(World& world, const glm::vec3& initialPos = glm::vec3(0.0f),
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			float initialYaw = 0.0f, float initialPitch = 0.0f, float width = 1.0f, float height = 1.0f);
		virtual ~Mob();
	protected:
		void tickAi() override;		
	};
}