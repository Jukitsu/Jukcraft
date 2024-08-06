#pragma once
#include "entity/Entity.h"
#include "models/Collider.h"

#include "physics/constants.h"

#include "animations/WalkAnimation.h"


namespace Jukcraft {

	class World;

	struct Health {
		float oldHp = 20.0f;
		float hp = 20.0f;
		float energy = 1.0f;

		void healNatural() {
			// hp = glm::min(hp + 0.25f / TICK_RATE, 20.0f);

		}

		bool hurt(float amount, bool isIframe) {
			if (!isIframe) {
				oldHp = hp;
				hp -= amount;
				LOG_INFO("{} {} {}", oldHp, hp, amount);;
			}
			else if (amount > oldHp - hp) {
				hp = oldHp - amount;
			}
			else {
				return false;
			}
			return true;

		}
	};

	class LivingEntity : public Entity {
	public:
		LivingEntity(World& world, const glm::vec3& initialPos = glm::vec3(0.0f), 
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			float initialYaw = 0.0f, float initialPitch = 0.0f, float width = 1.0f, float height = 1.0f);
		virtual ~LivingEntity();

		void jump(float jumpHeight = JUMP_HEIGHT);
		void tick() override;
		void hurt(float amount, const glm::vec3& knockback) override;

		void move(const glm::vec3& motion) override;
		void push(const glm::vec3& motion) override;

		void setInput(const glm::vec3& inputAccel) { input = inputAccel; }

		constexpr float getEyeLevel() const { return height - 0.2f; }
		constexpr const Collider& getCollider() const { return collider; }
		constexpr const glm::vec3& getInput() const { return input; }
		constexpr size_t getAge() const { return age;  }
		constexpr const glm::vec2& getBodyRot() const { return bodyRot; }
		constexpr const glm::vec2& getHeadRot() const { return headRot; }
		constexpr float getKineticEnergy() const { return 0.5f * mass * glm::dot(inertia, inertia); }
		constexpr float getMaxKineticEnergy() const { return mass * MAX_KINETIC_ENERGY_PER_MASS; }
		constexpr const auto& getOld() const { return old; }
		constexpr const auto& getAnimationTicks() const { return animationTicks; }
		constexpr const auto& getWalkAnimation() const { return walkAnimation; }
		constexpr bool isHurt() const { return iframes > 0 || deathTime > 0; }
		constexpr int getDeathTime() const { return deathTime; }

		void setHeadYaw(float theta) { headRot.x = wrapRadians(theta); }
		void setHeadPitch(float phi) { headRot.y = glm::clamp(phi, -glm::pi<float>() / 2, glm::pi<float>() / 2); }
		void setBodyYaw(float theta) { bodyRot.x = wrapRadians(theta);}

		void consumeInertia();
		void die();
	protected:
		void animate() override;
		void aiStep() override;
		void applyPhysics() override;
		void tickRotations() override;
		void checkInjury();
		constexpr const glm::vec3& getFriction() const {
			if (onGround)
				return FRICTION;
			else if (velocity.y > 0)
				return DRAG_JUMP;
			else
				return DRAG_FALL;
		}
	private:
		void updateCollider();
		void resolveCollisions();
	public:
		bool hasImpulse = false;
		bool onGround = false;
		bool onWall = false;
		int iframes = 0;
		int deathTime = 0;
	protected:
		glm::vec3 input;
		glm::vec2 bodyRot;
		glm::vec2 headRot;

		float width, height;

		float pendingInjury = 0.0f;
		int injuryCooldown = 0;

		Health health;
		float stamina = 1.0f;
		glm::vec3 inertia = glm::vec3(0.0f);

		size_t age = 0;

		const float speed;

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

		WalkAnimation walkAnimation;

		Collider collider;
		World& world;
	};
}