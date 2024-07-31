#pragma once

namespace Jukcraft {
	class Entity {
	public:
		Entity() :mass(1.0f) {

		}
		Entity(const glm::vec3& initialPos, const glm::vec3& initialVelocity, 
			float initialYaw, float initialPitch, float mass)
			:position(initialPos), velocity(initialVelocity),
			rotation(initialYaw, initialPitch), mass(mass) {

		}
		virtual ~Entity() {}

		virtual void tick() = 0;
		virtual void applyPhysics() = 0;
		virtual void aiStep() = 0;
		virtual void handleRotation() {}
		virtual void move(const glm::vec3& motion) = 0;
		virtual void push(const glm::vec3& motion) = 0;
		virtual void render(float partialTicks) {}

		constexpr const glm::vec3& getPos() const { return position; }
		constexpr const glm::vec3& getVelocity() const { return velocity; }
		constexpr float getYaw() const { return rotation.x; }
		constexpr float getPitch() const { return rotation.y; }
		constexpr const glm::vec2& getRotation() const { return rotation; }
		constexpr float getKineticEnergy() const { return 0.5f * mass * glm::dot(velocity, velocity); }

		void setPos(const glm::vec3& pos) { position = pos; }
		void setVelocity(const glm::vec3& v) { velocity = v; }
		void setYaw(float theta) { rotation.x = wrapRadians(theta); }
		void setPitch(float phi) { rotation.y = glm::clamp(phi, -glm::pi<float>() / 2, glm::pi<float>() / 2); }
	protected:
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec2 rotation;

		const float mass;
	};
}