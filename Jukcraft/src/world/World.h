#pragma once
#include "world/chunk/ChunkManager.h"
#include "world/LightEngine.h"

namespace Jukcraft {
	class World {
	public:
		World(const std::vector<Block>& blocks, gfx::Shader& shader);
		void tick(float deltaTime);
		void render();
		void setBlock(const glm::ivec3& worldPos, BlockID block);

		BlockID getBlock(const glm::ivec3& worldPos) const;

		void speedTime() {
			if (daylight <= 480)
				daylightIncrementer = 1;
			else if (daylight >= 1800)
				daylightIncrementer = -1;
		}

		void updateDaylight() {
			if (daylightIncrementer == -1 && daylight < 480) // Moonlight of 480/1800 * 15 = 4
				daylightIncrementer = 0;


			else if (daylightIncrementer == 1 && daylight >= 1800)
				daylightIncrementer = 0;

			if (time % 36000 == 0)
				daylightIncrementer = 1;
			else if (time % 36000 == 18000)
				daylightIncrementer = -1;

			daylight += daylightIncrementer;
		}

		[[nodiscard]] constexpr glm::vec4 getSkyColor() const {
			return glm::vec4(
				0.5f * ((float)daylight / 1800 - 0.26f),
				0.8f * ((float)daylight / 1800 - 0.26f),
				1.36f * ((float)daylight / 1800 - 0.26f),
				1.0f
			);
		}
	private:
		const std::vector<Block>& blocks;
		gfx::Shader& shader;
		ChunkManager chunkManager;
		LightEngine lightEngine;
		int daylight;
		int daylightIncrementer = 1;
		uint64_t time;
	};
}