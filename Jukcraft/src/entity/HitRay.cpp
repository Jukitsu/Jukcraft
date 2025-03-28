#include "pch.h"
#include "entity/HitRay.h"


namespace Jukcraft {
	HitRay::HitRay(World& world, LivingEntity& entity)
		:world(world), position(entity.getPos() - glm::vec3(0.5f) + glm::vec3(0, entity.getEyeLevel(), 0)), block((glm::ivec3)glm::round(entity.getPos() - glm::vec3(0.5f) + glm::vec3(0, entity.getEyeLevel(), 0))), distance(0.0f) {
		vector = entity.getEyesight();
	}

	bool HitRay::check(int button, HitCallback& callback,
		float distance, const BlockPos& currentBlock, const BlockPos& nextBlock) {

		if (world.getBlock(nextBlock)) {
			callback(button, currentBlock, nextBlock);
			return true;
		}
		else {
			position = position + vector * distance;
			block = nextBlock;
			this->distance += distance;
			return false;
		}
	}

	bool HitRay::step(int button, HitCallback callback) {
		glm::ivec3 sign = glm::sign(vector);
		glm::vec3 localPos = (glm::vec3)sign * (position - (glm::vec3)block);

		glm::vec3 absoluteVector = glm::abs(vector);


		float vx = absoluteVector.x, vy = absoluteVector.y, vz = absoluteVector.z;
		float lx = localPos.x, ly = localPos.y, lz = localPos.z;


		if (vx) {
			float x = 0.5f;
			float y = (0.5f - lx) / vx * vy + ly;
			float z = (0.5f - lx) / vx * vz + lz;

			if (y >= -0.5f && y <= 0.5f && z >= -0.5f && z <= 0.5f) {
				float distance = glm::distance(glm::vec3(x, y, z), localPos);
				return check(button, callback, distance, block, glm::ivec3(block.x + sign.x, block.y, block.z));
			}
		}

		if (vy) {
			float x = (0.5f - ly) / vy * vx + lx;
			float y = 0.5f;
			float z = (0.5f - ly) / vy * vz + lz;

			if (x >= -0.5f && x <= 0.5f && z >= -0.5f && z <= 0.5f) {
				float distance = glm::distance(glm::vec3(x, y, z), localPos);
				return check(button, callback, distance, block, glm::ivec3(block.x, block.y + sign.y, block.z));
			}
		}

		if (vz) {
			float x = (0.5f - lz) / vz * vx + lx;
			float y = (0.5f - lz) / vz * vy + ly;
			float z = 0.5f;

			if (x >= -0.5f && x <= 0.5f && y >= -0.5f && y <= 0.5f) {
				float distance = glm::distance(glm::vec3(x, y, z), localPos);
				return check(button, callback, distance, block, glm::ivec3(block.x, block.y, block.z + sign.z));
			}
		}

		return 0;
	}
}
