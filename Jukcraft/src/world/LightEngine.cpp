#include "pch.h"
#include "world/LightEngine.h"

namespace Jukcraft {

	void LightEngine::initSkyLight(std::shared_ptr<Chunk>& chunk) {
		glm::ivec2 chunkPos = chunk->getChunkPos();

		int32_t height = 0;
		for (uint32_t lx = 0; lx < CHUNK_DIM; lx++)
			for (uint32_t lz = 0; lz < CHUNK_DIM; lz++) {
				int32_t ly = CHUNK_HEIGHT - 1;
				for (; ly >= 0; ly--) {
					if (chunk->getBlock(BlockPos(lx, ly, lz)))
						break;

				}

				if (ly > height && ly >= 0)
					height = ly;
			}

		for (int32_t ly = CHUNK_HEIGHT - 1; ly >= height; ly--)
			for (uint32_t lx = 0; lx < CHUNK_DIM; lx++)
				for (uint32_t lz = 0; lz < CHUNK_DIM; lz++) {
					chunk->setSkyLight(BlockPos(lx, ly, lz), 15);
				}


		for (uint32_t lx = 0; lx < CHUNK_DIM; lx++)
			for (uint32_t lz = 0; lz < CHUNK_DIM; lz++) {
				glm::ivec3 localPos(lx, height, lz);
				BlockPos lightEnginePos(chunkPos, localPos);
				skylightIncreaseQueue.emplace(lightEnginePos, 15);
				propagateSkyLightIncrease();
			}
	}

	void LightEngine::increaseLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk, uint8_t light) {
		chunk->setBlockLight(pos.getLocalPos(), light);

		lightIncreaseQueue.emplace(pos, light);
		propagateLightIncrease();
	}

	void LightEngine::decreaseLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk) {
		uint8_t oldlight = chunk->getBlockLight(pos.getLocalPos());
		chunk->setBlockLight(pos.getLocalPos(), 0);

		lightDecreaseQueue.emplace(pos, oldlight);
		propagateLightDecrease();
		propagateLightIncrease();
	}

	void LightEngine::decreaseSkyLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk) {
		uint8_t oldlight = chunk->getSkyLight(pos.getLocalPos());
		chunk->setSkyLight(pos.getLocalPos(), 0);

		skylightDecreaseQueue.emplace(pos, oldlight);
		propagateSkyLightDecrease();
		propagateSkyLightIncrease();
	}

	void LightEngine::propagateLightIncrease() {
		while (!lightIncreaseQueue.empty()) {
			const BlockLightIncreaseNode& node = lightIncreaseQueue.front();
			BlockPos pos = node.pos;
			uint8_t light = node.light;
			lightIncreaseQueue.pop();


			for (const glm::ivec3& direction : IDIRECTIONS) {
				BlockPos newPos = pos + direction;

				if (newPos.y < 0 || newPos.y > CHUNK_HEIGHT)
					continue;

				std::optional<std::shared_ptr<Chunk>> chunk = chunkManager.getChunk(newPos.getChunkPos());
				if (!chunk.has_value())
					continue;

				glm::ivec3 localPos = newPos.getLocalPos();
				if (chunk.value()->getBlockLight(localPos) + 2 <= light 
						&& blocks[chunk.value()->getBlock(localPos)].isTransparent()) {
					chunk.value()->setBlockLight(localPos, light - 1);
					lightIncreaseQueue.emplace(newPos, light - 1);
					markPositionForUpdate(chunk.value(), localPos);
				}
			}
		}
	}

	void LightEngine::propagateSkyLightIncrease() {
		while (!skylightIncreaseQueue.empty()) {
			const SkyLightIncreaseNode& node = skylightIncreaseQueue.front();
			BlockPos pos = node.pos;
			uint8_t skylight = node.light;
			skylightIncreaseQueue.pop();


			for (const glm::ivec3& direction : IDIRECTIONS) {
				BlockPos newPos = pos + direction;
				if (newPos.y < 0 || newPos.y > CHUNK_HEIGHT)
					continue;

				std::optional<std::shared_ptr<Chunk>> chunk = chunkManager.getChunk(newPos.getChunkPos());
				if (!chunk.has_value())
					continue;
				glm::ivec3 localPos = newPos.getLocalPos();

				if (chunk.value()->getSkyLight(localPos) < skylight 
					&& blocks[chunk.value()->getBlock(localPos)].isTransparent()) {
					if (direction.y == -1) {
						chunk.value()->setSkyLight(localPos, skylight);
						skylightIncreaseQueue.emplace(newPos, skylight);
					}
					else if (chunk.value()->getSkyLight(localPos) + 2 <= skylight) {
						chunk.value()->setSkyLight(localPos, skylight - 1);
						skylightIncreaseQueue.emplace(newPos, skylight - 1);
					}
					else continue;
					markPositionForUpdate(chunk.value(), localPos);
				}
			}
		}
	}

	void LightEngine::propagateLightDecrease() {
		while (!lightDecreaseQueue.empty()) {
			const BlockLightDecreaseNode& node = lightDecreaseQueue.front();
			BlockPos pos = node.pos;
			uint8_t oldlight = node.oldlight;
			lightDecreaseQueue.pop();


			for (const glm::ivec3& direction : IDIRECTIONS) {
				BlockPos newPos = pos + direction;

				if (newPos.y < 0 || newPos.y > CHUNK_HEIGHT)
					continue;

				std::optional<std::shared_ptr<Chunk>> chunk = chunkManager.getChunk(newPos.getChunkPos());
				if (!chunk.has_value())
					continue;
				glm::ivec3 localPos = newPos.getLocalPos();
				Block block = blocks[chunk.value()->getBlock(localPos)];

				if (block.getLight()) {
					lightIncreaseQueue.emplace(newPos, block.getLight());
					continue;
				}

				if (block.isTransparent()) {
					uint8_t neighbourLight = chunk.value()->getBlockLight(localPos);
					if (!neighbourLight)
						continue;

					if (neighbourLight < oldlight) {
						chunk.value()->setBlockLight(localPos, 0);
						lightDecreaseQueue.emplace(newPos, neighbourLight);
						markPositionForUpdate(chunk.value(), localPos);
					}
					else if (neighbourLight >= oldlight) {
						lightIncreaseQueue.emplace(newPos, neighbourLight);
					}
				}

			}
		}
	}

	void LightEngine::propagateSkyLightDecrease() {
		while (!skylightDecreaseQueue.empty()) {
			const SkyLightDecreaseNode& node = skylightDecreaseQueue.front();
			BlockPos pos = node.pos;
			uint8_t oldlight = node.oldlight;
			skylightDecreaseQueue.pop();


			for (const glm::ivec3& direction : IDIRECTIONS) {
				BlockPos newPos = pos + direction;
				if (newPos.y < 0 || newPos.y > CHUNK_HEIGHT)
					continue;

				std::optional<std::shared_ptr<Chunk>> chunk = chunkManager.getChunk(newPos.getChunkPos());
				if (!chunk.has_value())
					continue;
				glm::ivec3 localPos = newPos.getLocalPos();
				Block block = blocks[chunk.value()->getBlock(localPos)];

				if (block.isTransparent()) {
					uint8_t neighbourSkylight = chunk.value()->getSkyLight(localPos);
					if (direction.y == -1 || neighbourSkylight < oldlight) {
						chunk.value()->setSkyLight(localPos, 0);
						skylightDecreaseQueue.emplace(newPos, neighbourSkylight);
						markPositionForUpdate(chunk.value(), localPos);
					}
					else if (neighbourSkylight >= oldlight) {
						skylightIncreaseQueue.emplace(newPos, neighbourSkylight);
					}
				}
			}
		}
	}

	void LightEngine::markPositionForUpdate(std::shared_ptr<Chunk>& chunk, const glm::ivec3& localPos) {
		if (doLightUpdates) {
			chunkManager.updateChunkAtPosition(chunk, localPos);
		}
	}
}