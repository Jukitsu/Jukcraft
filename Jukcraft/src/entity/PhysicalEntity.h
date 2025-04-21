#pragma once
#include "entity/Entity.h"
#include "physics/constants.h"

namespace Jukcraft {

	class World;
	
	class PhysicalEntity : public Entity {
	public:
		PhysicalEntity(World& world, const glm::vec3& initialPos = glm::vec3(0.0f),
			const glm::vec3& initialVelocity = glm::vec3(0.0f),
			float initialYaw = 0.0f, float initialPitch = 0.0f, float width = 1.0f, float height = 1.0f);

		~PhysicalEntity();
		bool isPhysical() override { return true; }

		void move(const glm::vec3& motion) override;
		void push(const glm::vec3& motion) override;
		void collide(const glm::vec3& motion) override;

	protected:
		void applyPhysics() override;

	private:
		void resolveCollisions();
		constexpr const glm::vec3& getFriction() const {
			if (onGround)
				return FRICTION;
			else if (velocity.y > 0)
				return DRAG_JUMP;
			else
				return DRAG_FALL;
		}
	public:
		
		bool hasCollision = false;
		bool hasImpulse = false;
		bool onGround = false;
		bool onWall = false;

		float absorbedEnergy = 0.0f;
		World& world;
	protected:

		float width, height;
	};
}