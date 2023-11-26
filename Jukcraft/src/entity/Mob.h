#pragma once
#include "entity/Entity.h"
#include "models/Collider.h"

namespace Jukcraft {
	class Mob : public Entity {
	public:
		Mob(const glm::vec3& initialPos = glm::vec3(0.0f), const glm::vec3& initialVelocity = glm::vec3(0.0f),
			const glm::vec3& initialAcceleration = glm::vec3(0.0f), float initialYaw = 0.0f, float initialPitch = 0.0f) 
			:Entity(initialPos, initialVelocity, initialAcceleration, initialYaw, initialPitch),
			width(0.6f), height(1.8f), collider(){

		}
		virtual ~Mob() {

		}
		void updateCollider() {
			collider.vx1 = position - glm::vec3(width / 2.0f, 0, width / 2.0f);
			collider.vx2 = position + glm::vec3(width / 2.0f, height, width / 2.0f);
		}

		void tick(float delta_time) override {
			updateCollider();

			acceleration += velocity * delta_time;
			position += velocity * delta_time;
		}
	protected:
		float width, height;

		Collider collider;
	};
}