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
		void handleRotation() override;
		void move(const glm::vec3& motion) override;
		void push(const glm::vec3& motion) override;

		void setInput(const glm::vec3& inputAccel) { input = inputAccel; }

		constexpr float getEyeLevel() const { return height - 0.2f; }
		constexpr const Collider& getCollider() const { return collider; }
		constexpr const glm::vec3& getInput() const { return input; }

		constexpr const glm::vec2& getBodyRot() { return bodyRot; }
		constexpr const glm::vec2& getHeadRot() { return headRot; }

		constexpr const auto& getOld() { return old; }

		void setHeadYaw(float theta) { headRot.x = wrapRadians(theta); }
		void setHeadPitch(float phi) { headRot.y = glm::clamp(phi, -glm::pi<float>() / 2, glm::pi<float>() / 2); }
		void setBodyYaw(float theta) { bodyRot.x = wrapRadians(theta);}
	protected:
		constexpr const glm::vec3& getFriction() const {
			if (onGround)
				return FRICTION;
			else if (velocity.y > 0)
				return DRAG_JUMP;
			else
				return DRAG_FALL;
		}

	protected:
		glm::vec3 input;
		glm::vec2 bodyRot;
		glm::vec2 headRot;

		float width, height;

		int stamina = 0;
		glm::vec3 inertia = glm::vec3(0.0f);

		const float speed;

		bool onGround;

		struct {
			glm::vec3 position;
			glm::vec2 bodyRot;
			glm::vec2 headRot;
			glm::vec3 velocity;
		} old;

		struct {
			float attack = 0.0f;
			float step = 0.0f;
		} animationTicks;

		Collider collider;
		World& world;
	};
}