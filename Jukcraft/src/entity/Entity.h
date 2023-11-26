#pragma once

namespace Jukcraft {
	class Entity {
	public:
		Entity() {

		}
		Entity(const glm::vec3& initialPos, const glm::vec3& initialVelocity,
			const glm::vec3& initialAcceleration, float initialYaw, float initialPitch)
			:position(initialPos), velocity(initialVelocity), acceleration(initialAcceleration),
			yaw(initialYaw), pitch(initialPitch) {

		}
		virtual ~Entity() {}

		virtual void tick(float delta_time) = 0;

		constexpr const glm::vec3& getPos() const { return position; }
		constexpr const glm::vec3& getVelocity() const { return position; }
		constexpr const glm::vec3& getAccel() const { return position; }
		constexpr float getYaw() const { return yaw; }
		constexpr float getPitch() const { return pitch; }
	protected:
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 acceleration;
		float yaw, pitch;
	};
}