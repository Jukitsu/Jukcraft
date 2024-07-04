#pragma once
#include "entity/Entity.h"
#include "models/Collider.h"

#include "physics/constants.h"

namespace Jukcraft {

	class World;

	class LivingEntity : public Entity {
	public:
		LivingEntity(World& world, const glm::vec3& initialPos = glm::vec3(0.0f), 
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			float initialYaw = 0.0f, float initialPitch = 0.0f, float width = 1.0f, float height = 1.0f);
		virtual ~LivingEntity();

		void updateCollider();
		void resolveCollisions();
		void jump(float jumpHeight = JUMP_HEIGHT);
		void aiStep() override;
		void tick() override;
		void applyPhysics() override;
		void move(const glm::vec3& motion) override;
		void push(const glm::vec3& motion) override;

		void setInput(const glm::ivec3& inputAccel) { input = inputAccel; }

		constexpr float getEyeLevel() const { return height - 0.2f; }
		constexpr const Collider& getCollider() const { return collider; }
		constexpr const glm::ivec3& getInput() const { return input; }
	protected:
		glm::ivec3 input;

		constexpr const glm::vec3& getFriction() const {
			if (onGround)
				return FRICTION;
			else if (velocity.y > 0)
				return DRAG_JUMP;
			else
				return DRAG_FALL;
		}

		float width, height;

		const float speed;

		bool onGround;

		glm::vec3 oldPosition;
		glm::vec3 interpolatedPos;
		float interpolationStep;

		Collider collider;
		World& world;
	};
}