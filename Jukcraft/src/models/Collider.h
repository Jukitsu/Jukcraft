#pragma once
namespace Jukcraft {

	struct Collider {
		glm::vec3 vx1;
		glm::vec3 vx2;

		Collider operator+(const glm::vec3& v) const;
		bool operator&(const Collider& other) const;

		std::pair<float, glm::vec3> collide(const Collider& collider, const glm::vec3& velocity) const;
	};

	inline Collider Collider::operator+(const glm::vec3& v) const{ 
		return Collider{ vx1 + v, vx2 + v }; 
	}

	inline bool Collider::operator&(const Collider& other) const {
		return (glm::min(vx2.x, other.vx2.x) - glm::max(vx1.x, other.vx1.x) > 0)
			&& (glm::min(vx2.y, other.vx2.y) - glm::max(vx1.y, other.vx1.y) > 0)
			&& (glm::min(vx2.z, other.vx2.z) - glm::max(vx1.z, other.vx1.z) > 0);
	}

	inline std::pair<float, glm::vec3> Collider::collide(const Collider& collider, const glm::vec3& velocity) const {
		const Collider& self = *this; // Just some syntactic sugar (totally no CTRL-V involved)

		// self: the dynamic collider, which moves
		// collider: the static collider, which stays put

		std::pair<float, glm::vec3> no_collision = { 1, { 0, 0, 0 } };

		// find entry & exit times for each axis

		auto time = [](float x, float y) {
			return y ? x / y : x > 0 ? std::numeric_limits<float>::min() : std::numeric_limits<float>::max();
			};


		float x_entry = time(velocity.x > 0 ? collider.vx1.x - self.vx2.x : collider.vx2.x - self.vx1.x, velocity.x);
		float x_exit = time(velocity.x > 0 ? collider.vx2.x - self.vx1.x : collider.vx1.x - self.vx2.x, velocity.x);

		float y_entry = time(velocity.y > 0 ? collider.vx1.y - self.vx2.y : collider.vx2.y - self.vx1.y, velocity.y);
		float y_exit = time(velocity.y > 0 ? collider.vx2.y - self.vx1.y : collider.vx1.y - self.vx2.y, velocity.y);

		float z_entry = time(velocity.x > 0 ? collider.vx1.z - self.vx2.z : collider.vx2.z - self.vx1.z, velocity.z);
		float z_exit = time(velocity.z > 0 ? collider.vx2.z - self.vx1.z : collider.vx1.z - self.vx2.z, velocity.z);

		// make sure we actually got a collision

		if (x_entry < 0 && y_entry < 0 && z_entry < 0)
			return no_collision;

		if (x_entry > 1 || y_entry > 1 || z_entry > 1)
			return no_collision;
		
		// on which axis did we collide first?

		float entry = glm::max(x_entry, y_entry, z_entry);
		float exit = glm::min(x_exit, y_exit, z_exit);

		if (entry > exit)
			return no_collision;
		
		// find normal of surface we collided with

		glm::vec3 normal = {
			entry != x_entry ? 0 : velocity.x > 0 ? -1 : 1,
			entry != y_entry ? 0 : velocity.y > 0 ? -1 : 1,
			entry != z_entry ? 0 : velocity.z > 0 ? -1 : 1
		};

		return { entry, normal };
	}
}