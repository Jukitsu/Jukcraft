#pragma once
#include "entity/BipedEntity.h"

namespace Jukcraft {
	class Player : public BipedEntity {
	public:
		Player(World& world, const glm::vec3& initialPos = glm::vec3(0.0f), const glm::vec3& initialVelocity = glm::vec3(0.0f),
			const glm::vec3& initialAcceleration = glm::vec3(0.0f), float initialYaw = 0.0f, float initialPitch = 0.0f);
		virtual ~Player();
	private:
		friend class Camera;
	};
}
