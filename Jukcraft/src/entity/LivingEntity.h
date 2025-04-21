#pragma once
#include "entity/PhysicalEntity.h"

#include "physics/constants.h"

#include "animations/WalkAnimation.h"


namespace Jukcraft {

	struct Health {
		float oldHp = 20.0f;
		float hp = 20.0f;
		float energy = 1.0f;

		void healNatural() {
			hp = glm::min(hp + 0.25f / TICK_RATE, 20.0f);

		}

		bool hurt(float amount, bool isIframe) {
			if (!isIframe) {
				oldHp = hp;
				hp -= amount;
				
			}
			else if (amount > oldHp - hp) {
				hp = oldHp - amount;
			}
			else {
				return false;
			}
			// LOG_INFO("{} {} {}", oldHp, hp, amount);
			return true;

		}
	};

	class LivingEntity : public PhysicalEntity {
	public:
		LivingEntity(World& world, const glm::vec3& initialPos = glm::vec3(0.0f), 
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			float initialYaw = 0.0f, float initialPitch = 0.0f, float width = 1.0f, float height = 1.0f);
		virtual ~LivingEntity();

		bool isPhysical() override { return deathTime <= 0; }

		void updateCollider() override;

		void tick() override;
		void hurt(float amount, const glm::vec3& knockback) override;

		void setInput(const glm::vec3& inputAccel) { input = inputAccel; }


		constexpr float getEyeLevel() const { return height - 0.2f; }
		constexpr const Collider& getCollider() const { return collider; }
		constexpr const glm::vec3& getInput() const { return input; }
		constexpr size_t getAge() const { return age;  }
		constexpr const glm::vec2& getBodyRot() const { return bodyRot; }
		constexpr const glm::vec2& getHeadRot() const { return headRot; }
		constexpr float getKineticEnergy() const { return 0.5f * mass * glm::dot(velocity, velocity); }
		constexpr float getMaxKineticEnergy() const { return mass * MAX_KINETIC_ENERGY_PER_MASS; }
		constexpr const auto& getOld() const { return old; }
		constexpr const auto& getAnimationTicks() const { return animationTicks; }
		constexpr const auto& getWalkAnimation() const { return walkAnimation; }
		constexpr bool isHurt() const { return iframes > 0 || deathTime > 0; }
		constexpr int getDeathTime() const { return deathTime; }
		
		constexpr bool isMoving() const {
			return glm::dot(old.position - position, old.position - position) > 1.0E-5f;
		}

		constexpr glm::vec3 getEyesight() const {
			return glm::vec3(
				glm::cos(rotation.x) * glm::cos(rotation.y),
				glm::sin(rotation.y),
				glm::sin(rotation.x) * glm::cos(rotation.y)
			);
		}

		void setHeadYaw(float theta) { headRot.x = wrapRadians(theta); }
		void setHeadPitch(float phi) { headRot.y = glm::clamp(phi, -glm::pi<float>() / 2, glm::pi<float>() / 2); }
		void setBodyYaw(float theta) { bodyRot.x = wrapRadians(theta);}

		void die();
		void setJumping(bool state);
	protected:
		void jump(float jumpHeight = JUMP_HEIGHT);
		void animate() override;
		void aiStep() override;
		void applyPhysics() override;
		void tickRotations() override;
		void checkInjury();
		virtual void tickAi() {}

		constexpr const glm::vec3& getFriction() const {
			if (onGround)
				return FRICTION;
			else if (velocity.y > 0)
				return DRAG_JUMP;
			else
				return DRAG_FALL;
		}
	private:
		void updateOldState();
	public:
		bool jumping = false;
		int iframes = 0;
		int deathTime = 0;
		
	protected:
		glm::vec3 input;
		glm::vec2 bodyRot;
		glm::vec2 headRot;

		Health health;
		float stamina = 1.0f;

		size_t age = 0;

		float speed;

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
	};
}