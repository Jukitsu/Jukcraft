#pragma once
#include "world/World.h"
#include "core/Camera.h"

using HitCallback = std::function<void(int, const glm::vec3&, const glm::vec3&)>;

struct HitRay {
	HitRay(World& world, Camera& camera);
	bool check(int button, HitCallback callback,
		float distance, const glm::vec3& currentBlock, const glm::vec3& nextBlock);
	bool step(int button, HitCallback callback);

	World& world;
	glm::vec3 vector;
	glm::vec3 position;
	glm::ivec3 block;
	float distance;
};

constexpr uint8_t HIT_RANGE = 3;