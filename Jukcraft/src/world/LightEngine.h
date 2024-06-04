#pragma once
#include "world/chunk/ChunkManager.h"

namespace Jukcraft {
	class LightEngine {
	public:
		LightEngine(ChunkManager& chunkManager, const std::vector<Block>& blocks)
			:chunkManager(chunkManager), blocks(blocks) {}
		void increaseLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk, uint8_t light);
		void initSkyLight(std::shared_ptr<Chunk>& chunk);
		void decreaseLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk);
		void decreaseSkyLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk);
		void propagateLightIncrease();
		void propagateLightDecrease();
		void propagateSkyLightIncrease();
		void propagateSkyLightDecrease();

		void toggleLightUpdates(bool value) noexcept { doLightUpdates = value; }
	private:
		void markPositionForUpdate(std::shared_ptr<Chunk>&, const glm::ivec3& localPos);
		struct BlockLightIncreaseNode {
			BlockPos pos;
			uint8_t light;
		};
		struct BlockLightDecreaseNode {
			BlockPos pos;
			uint8_t oldlight;
		};
		struct SkyLightIncreaseNode {
			BlockPos pos;
			uint8_t light;
		};
		struct SkyLightDecreaseNode {
			BlockPos pos;
			uint8_t oldlight;
		};
		std::queue<BlockLightIncreaseNode> lightIncreaseQueue;
		std::queue<SkyLightIncreaseNode> skylightIncreaseQueue;
		std::queue<BlockLightDecreaseNode> lightDecreaseQueue;
		std::queue<SkyLightDecreaseNode> skylightDecreaseQueue;

		ChunkManager& chunkManager;
		const std::vector<Block>& blocks;
		bool doLightUpdates = false;
	};
}