#pragma once
#include "world/World.h"
#include "core/Camera.h"


namespace Jukcraft {
	using HitCallback = std::function<void(int, const BlockPos&, const BlockPos&)>;

	struct HitRay {
		HitRay(World& world, LivingEntity& entity);
		bool check(int button, HitCallback& callback,
			float distance, const BlockPos& currentBlock, const BlockPos& nextBlock);
		bool step(int button, HitCallback callback);

		World& world;
		glm::vec3 vector;
		glm::vec3 position;
		BlockPos block;
		float distance;
	};

	constexpr uint8_t HIT_RANGE = 3;

}