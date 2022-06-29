#include "pch.h"
#include "core/HitRay.h"

HitRay::HitRay(World& world, Camera& camera) 
	:world(world), position(camera.getPos()), block((glm::ivec3)glm::floor(camera.getPos())), distance(0.0f) {
	vector = glm::vec3(
		glm::cos(camera.getYaw()) * glm::cos(camera.getPitch()),
		glm::sin(camera.getPitch()),
		glm::sin(camera.getYaw()) * glm::cos(camera.getPitch())
	);
}

bool HitRay::check(int button, HitCallback callback,
	float distance, const glm::vec3& currentBlock, const glm::vec3& nextBlock) {

	if (world.getBlock(nextBlock)) {
		callback(button, currentBlock, nextBlock);
		return true;
	}
	else {
		position = position + vector * distance;
		block = nextBlock;
		LOG_TRACE("{} {} {}", nextBlock.x, nextBlock.y, nextBlock.z);
		this->distance += distance;
		return false;
	}
}
bool HitRay::step(int button, HitCallback callback) {
	glm::ivec3 sign = { 1, 1, 1 };
	glm::vec3 localPos = position - (glm::vec3)block;

	glm::vec3 absoluteVector = vector;

	for (uint8_t component = 0; component < 3; component++) {
		if (vector[component] < 0) {
			sign[component] = -1;

			absoluteVector[component] = -absoluteVector[component];
			localPos[component] = -localPos[component];
		}
	}
		
	float vx = absoluteVector.x, vy = absoluteVector.y, vz = absoluteVector.z;
	float lx = localPos.x, ly = localPos.y, lz = localPos.z;
				

	if (vx) {
		float x = 1.0f;
		float y = (1.0f - lx) / vx * vy + ly;
		float z = (1.0f - lx) / vx * vz + lz;

		if (y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) {
			float distance = glm::distance(glm::vec3(x, y, z), localPos);
			return check(button, callback, distance, block, glm::vec3(block.x + sign.x, block.y, block.z));
		}
	}
		
	if (vy) {
		float x = (1.0f - ly) / vy * vx + lx;
		float y = 1.0f;
		float z = (1.0f - ly) / vy * vz + lz;

		if (x >= 0.0f && x <= 1.0f && z >= 0.0f && z <= 1.0f) {
			float distance = glm::distance(glm::vec3(x, y, z), localPos);
			return check(button, callback, distance, block, glm::vec3(block.x, block.y + sign.y, block.z));
		}
	}
	
	if (vz) {
		float x = (1.0f - lz) / vz * vx + lx;
		float y = (1.0f - lz) / vz * vy + ly;
		float z = 1.0f;

		if (x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f) {
			float distance = glm::distance(glm::vec3(x, y, z), localPos);
			return check(button, callback, distance, block, glm::vec3(block.x, block.y, block.z + sign.z));
		}
	}
}
