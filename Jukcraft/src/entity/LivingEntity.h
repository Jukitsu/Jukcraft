#pragma once
#include "entity/Entity.h"
#include "models/Collider.h"

namespace Jukcraft {

	class World;

	class LivingEntity : public Entity {
	public:
		LivingEntity(World& world, const glm::vec3& initialPos = glm::vec3(0.0f), 
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			const glm::vec3& initialAcceleration = glm::vec3(0.0f), 
			float initialYaw = 0.0f, float initialPitch = 0.0f, float width = 1.0f, float height = 1.0f);
		virtual ~LivingEntity();

		void updateCollider();
		void resolveCollisions(float deltaTime);
		void tick(float deltaTime) override;

		constexpr float getEyeLevel() const { return height - 0.2f; }
		constexpr const Collider& getCollider() const { return collider; }
	protected:
		float width, height;

		glm::vec3 oldPosition;
		glm::vec3 interpolatedPos;
		float interpolationStep;

		Collider collider;
		World& world;
	};
}