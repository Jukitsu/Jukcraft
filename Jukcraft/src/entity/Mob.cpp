#include "pch.h"
#include "entity/Mob.h"

#include "core/App.h"

namespace Jukcraft {
	Mob::Mob(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		float initialYaw, float initialPitch, float width, float height)
		:LivingEntity(world, initialPos, initialVelocity, initialYaw, initialPitch, width, height)
	{

	}
	Mob::~Mob() {

	}

	void Mob::aiStep() {

		if (deathTime <= 0) {
			if (randomContinuous(0, 100) > 99) {
				float dx = randomContinuous(0, 12) - 6;
				float dz = randomContinuous(0, 12) - 6;

				if (dx != 0 && dz != 0) {
					setYaw(wrapRadians(glm::atan(dz, dx) - glm::pi<float>() / 2));
				}
			}

			float relativeAngle = wrapRadians(rotation.x - headRot.x);

			if (glm::abs(relativeAngle) > 0) {
				headRot.x += relativeAngle * 2.0f / TICK_RATE;
			}

			if (1) {
				setInput(glm::vec3(0.0f, 0.0f, 1.0f));
			}

			if (App::GetWindow().isKeyPressed(GLFW_KEY_LEFT_SHIFT) && onGround) {
				hurt(5.0f, glm::vec3(
					-20 / TICK_RATE * glm::cos(rotation.x),
					10 / TICK_RATE,
					-20 / TICK_RATE * glm::sin(rotation.x)
				));
			}
		}
		

		LivingEntity::aiStep();

		if (onWall) {
			jump();
		}
	}
}