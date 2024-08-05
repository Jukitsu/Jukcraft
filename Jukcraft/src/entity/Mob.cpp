#include "pch.h"
#include "entity/Mob.h"

#include "core/App.h"

namespace Jukcraft {
	Mob::Mob(World& world, const glm::vec3& initialPos, const glm::vec3& initialVelocity,
		float initialYaw, float initialPitch, float width, float height)
		:LivingEntity(world, initialPos, initialVelocity, initialYaw, initialPitch, width, height),
		dev(), rng(dev())
	{

	}
	Mob::~Mob() {

	}

	void Mob::aiStep() {

		if (deathTime <= 0) {
			std::uniform_int_distribution<std::mt19937::result_type> uniformDist2(0, 100);
			if (uniformDist2(rng) > 99) {
				std::uniform_int_distribution<std::mt19937::result_type> uniformDist(0, 12);
				float dx = (int)uniformDist(rng) - 6;
				float dz = (int)uniformDist(rng) - 6;

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
				hurt(19.9f, glm::vec3(
					-20 / TICK_RATE * glm::cos(rotation.x),
					10 / TICK_RATE,
					-20 / TICK_RATE * glm::sin(rotation.x)
				));
			}

			// LOG_INFO("{}", health.hp);
		}
		

		

		LivingEntity::aiStep();
	}
}