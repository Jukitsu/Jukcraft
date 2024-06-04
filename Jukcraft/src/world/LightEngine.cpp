#include "pch.h"
#include "world/LightEngine.h"

namespace Jukcraft {

	void LightEngine::initSkyLight(std::shared_ptr<Chunk>& chunk) {
		glm::ivec2 chunkPos = chunk->getChunkPos();

		uint32_t height = 0;
		for (uint32_t lx = 0; lx < CHUNK_DIM; lx++)
			for (uint32_t lz = 0; lz < CHUNK_DIM; lz++) {
				int ly = CHUNK_HEIGHT - 1;
				for (; ly >= 0; ly--) {
					if (chunk->getBlock(BlockPos(lx, ly, lz)))
						break;

				}

				if (ly > (int)height && ly >= 0)
					height = (uint32_t)ly;
			}

		for (uint32_t ly = CHUNK_HEIGHT - 1; ly >= height; ly--)
			for (uint32_t lx = 0; lx < CHUNK_DIM; lx++)
				for (uint32_t lz = 0; lz < CHUNK_DIM; lz++) {
					chunk->setSkyLight(BlockPos(lx, ly, lz), 15);
				}


		for (uint32_t lx = 0; lx < CHUNK_DIM; lx++)
			for (uint32_t lz = 0; lz < CHUNK_DIM; lz++) {
				glm::ivec3 localPos(lx, height, lz);
				BlockPos lightEnginePos(chunkPos, localPos);
				skylightIncreaseQueue.push(
					SkyLightIncreaseNode{
						lightEnginePos,
						15
					}
				);
				propagateSkyLightIncrease();
			}
	}

	void LightEngine::increaseLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk, uint8_t light) {
		chunk->setBlockLight(pos.getLocalPos(), light);

		lightIncreaseQueue.push(
			BlockLightIncreaseNode{
				pos,
				light
			}
		);
		propagateLightIncrease();
	}

	void LightEngine::decreaseLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk) {
		uint8_t oldlight = chunk->getBlockLight(pos.getLocalPos());
		chunk->setBlockLight(pos.getLocalPos(), 0);

		lightDecreaseQueue.push(
			BlockLightDecreaseNode{
				pos,
				oldlight
			}
		);
		propagateLightDecrease();
		propagateLightIncrease();
	}

	void LightEngine::decreaseSkyLight(const BlockPos& pos, std::shared_ptr<Chunk>& chunk) {
		uint8_t oldlight = chunk->getSkyLight(pos.getLocalPos());
		chunk->setSkyLight(pos.getLocalPos(), 0);

		skylightDecreaseQueue.push(
			SkyLightDecreaseNode{
				pos,
				oldlight
			}
		);
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
					lightIncreaseQueue.push(
						BlockLightIncreaseNode{
							newPos,
							(uint8_t)(light - 1)
						}
					);
					chunkManager.updateChunkAtPosition(chunk.value(), localPos);
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
						skylightIncreaseQueue.push(
							SkyLightIncreaseNode{
								newPos,
								skylight
							}
						);
					}
					else if (chunk.value()->getSkyLight(localPos) + 2 <= skylight) {
						chunk.value()->setSkyLight(localPos, skylight - 1);
						skylightIncreaseQueue.push(
							SkyLightIncreaseNode{
								newPos,
								(uint8_t)(skylight - 1)
							}
						);
					}
					else continue;
					chunkManager.updateChunkAtPosition(chunk.value(), localPos);
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
					lightIncreaseQueue.push(
						BlockLightIncreaseNode{
							newPos,
							block.getLight()
						}
					);
					continue;
				}

				if (block.isTransparent()) {
					uint8_t neighbourLight = chunk.value()->getBlockLight(localPos);
					if (!neighbourLight)
						continue;

					if (neighbourLight < oldlight) {
						chunk.value()->setBlockLight(localPos, 0);
						lightDecreaseQueue.push(
							BlockLightDecreaseNode{
								newPos,
								neighbourLight
							}
						);
						chunkManager.updateChunkAtPosition(chunk.value(), localPos);
					}
					else if (neighbourLight >= oldlight) {
						lightIncreaseQueue.push(
							BlockLightIncreaseNode{
								newPos,
								neighbourLight
							}
						);
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
						skylightDecreaseQueue.push(
							SkyLightDecreaseNode{
								newPos,
								neighbourSkylight
							}
						);
						chunkManager.updateChunkAtPosition(chunk.value(), localPos);
					}
					else if (neighbourSkylight >= oldlight) {
						skylightIncreaseQueue.push(
							SkyLightIncreaseNode{
								newPos,
								neighbourSkylight
							}
						);
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