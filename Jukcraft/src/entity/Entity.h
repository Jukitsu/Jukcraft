#pragma once

namespace Jukcraft {
	class Entity {
	public:
		Entity() :mass(1.0f) {

		}
		Entity(const glm::vec3& initialPos, const glm::vec3& initialVelocity,
			const glm::vec3& initialAcceleration, float initialYaw, float initialPitch, float mass)
			:position(initialPos), velocity(initialVelocity), relativeAccel(initialAcceleration),
			yaw(initialYaw), pitch(initialPitch), mass(mass) {

		}
		virtual ~Entity() {}

		virtual void tick() = 0;
		virtual void applyPhysics() = 0;
		virtual void aiStep() = 0;
		virtual void move(const glm::vec3& motion) = 0;
		virtual void push(const glm::vec3& motion) = 0;
		virtual void render(float deltaTime) {}

		constexpr const glm::vec3& getPos() const { return position; }
		constexpr const glm::vec3& getVelocity() const { return velocity; }
		constexpr const glm::vec3& getRelativeAccel() const { return relativeAccel; }
		constexpr float getYaw() const { return yaw; }
		constexpr float getPitch() const { return pitch; }
		constexpr float getKineticEnergy() const { return 0.5f * mass * velocity.length() * velocity.length(); }

		void setPos(const glm::vec3& pos) { position = pos; }
		void setVelocity(const glm::vec3& v) { velocity = v; }
		void setRelativeAccel(const glm::vec3& a) { relativeAccel = a; }
		void setYaw(float theta) { yaw = theta; }
		void setPitch(float phi) { pitch = phi; }
	protected:
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 relativeAccel;
		float yaw, pitch;
		const float mass;
	};
}