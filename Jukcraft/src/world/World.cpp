#include "pch.h"
#include "world/World.h"

namespace Jukcraft {
	World::World(const std::vector<Block>& blocks, gfx::Shader& shader)
		:chunkManager(blocks), time(0), shader(shader), blocks(blocks), lightEngine(chunkManager, blocks), daylight(1.0f) {

		for (std::shared_ptr<Chunk> chunk : chunkManager.getSkyLightPendingChunks()) {
			lightEngine.initSkyLight(chunk);
		}
	}

	void World::tick(float deltaTime) {
		time += 1;
		updateDaylight();
		chunkManager.tick();
		lightEngine.propagateLightIncrease();
	}


	void World::setBlock(const glm::ivec3& worldPos, BlockID blockID) {
		glm::ivec2 chunkPos = Chunk::ToChunkPos(worldPos);
		std::optional<std::shared_ptr<Chunk>> pendingChunk = chunkManager.getChunk(chunkPos);
		if (!pendingChunk.has_value())
			return;
		std::shared_ptr<Chunk>& chunk = *pendingChunk;
		glm::ivec3 localPos = Chunk::ToLocalPos(worldPos);
		const Block& block = blocks[blockID];
		chunk->setBlock(localPos, blockID);

		if (!blockID) {
			lightEngine.decreaseLight(worldPos, chunk);
			lightEngine.decreaseSkyLight(worldPos, chunk);
		}
		else {
			if (block.getLight()) {
				lightEngine.increaseLight(worldPos, chunk, block.getLight());
			}
			else if (!block.isTransparent()) {
				lightEngine.decreaseLight(worldPos, chunk);
				lightEngine.decreaseSkyLight(worldPos, chunk);
			}
		}

		chunkManager.updateChunkAtPosition(chunk, localPos);
	}

	BlockID World::getBlock(const glm::ivec3& worldPos) const {
		glm::ivec2 chunkPos = Chunk::ToChunkPos(worldPos);
		std::optional<std::shared_ptr<const Chunk>> pendingChunk = chunkManager.getChunk(chunkPos);
		if (!pendingChunk.has_value())
			return 0;
		std::shared_ptr<const Chunk>& chunk = *pendingChunk;
		glm::ivec3 localPos = Chunk::ToLocalPos(worldPos);
		return chunk->getBlock(localPos);
	}

	void World::render() {
		shader.setUniform1f(1, (float)daylight / 1800);
		chunkManager.drawChunksCubeLayers(shader);
	}


}