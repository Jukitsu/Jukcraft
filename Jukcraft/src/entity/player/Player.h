#pragma once
#include "entity/LivingEntity.h"

namespace Jukcraft {
	class Player : public LivingEntity {
	public:
		Player(World& world, const glm::vec3& initialPos = glm::vec3(0.0f), 
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			float initialYaw = 0.0f, float initialPitch = 0.0f);
		virtual ~Player();

		void tickAi() override;
		void dash();

		constexpr bool isDashing() const { return dashing; }
	private:
		bool dashing = false;
		float fov = glm::radians(70.0f);
		float fovOld = glm::radians(70.0f);
		int16_t dashCooldown = 0;
		friend class Camera;
	};
}
