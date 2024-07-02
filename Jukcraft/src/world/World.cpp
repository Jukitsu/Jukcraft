#include "pch.h"
#include "world/World.h"

namespace Jukcraft {
	World::World(const std::vector<Block>& blocks, gfx::Shader& shader)
		:chunkManager(blocks), time(0), shader(shader), blocks(blocks), lightEngine(chunkManager, blocks), daylight(1800) {

		/* Initialize lighting */
		std::future<void> result = std::async(
			std::launch::async,
			[&]() {
				for (Shared<Chunk> chunk : chunkManager.getSkyLightPendingChunks()) {
					lightEngine.initSkyLight(chunk);
				}
			}
		);

		lightEngine.toggleLightUpdates(true);
	}

	void World::tick() {
		time += 1;
		updateDaylight();
		chunkManager.tick();
		lightEngine.propagateLightIncrease(); // Always check for any pending light updates
	}

	void World::trySetBlock(Player& player, const BlockPos& worldPos, BlockID blockID) {
		if (!blockID)
			setBlock(worldPos, blockID);

		for (const Collider& collider : blocks[blockID].getColliders()) 
			if (player.getCollider() & (collider + worldPos)) // Do not place a block at the player's feet
				return; 
		
		setBlock(worldPos, blockID);
	}
	void World::setBlock(const BlockPos& blockPos, BlockID blockID) {
		glm::ivec2 chunkPos = blockPos.getChunkPos();
		Nullable<Shared<Chunk>>&& pendingChunk = chunkManager.getChunk(chunkPos);

		if (!pendingChunk.has_value())
			return;

		Shared<Chunk>& chunk = *pendingChunk;
		glm::ivec3 localPos = blockPos.getLocalPos();
		const Block& block = blocks[blockID];
		chunk->setBlock(localPos, blockID);

		if (!blockID) {
			lightEngine.decreaseLight(blockPos, chunk);
			lightEngine.decreaseSkyLight(blockPos, chunk);
		}
		else {
			if (block.getLight()) {
				lightEngine.increaseLight(blockPos, chunk, block.getLight());
			}
			else if (!block.isTransparent()) {
				lightEngine.decreaseLight(blockPos, chunk);
				lightEngine.decreaseSkyLight(blockPos, chunk);
			}
		}

		chunkManager.updateChunkAtPosition(chunk, localPos); // Chunk Update
	}

	BlockID World::getBlock(const BlockPos& blockPos) const {
		glm::ivec2 chunkPos = blockPos.getChunkPos();
		Nullable<Shared<const Chunk>>&& pendingChunk = chunkManager.getChunk(chunkPos);
		if (!pendingChunk.has_value())
			return 0;
		Shared<const Chunk>& chunk = *pendingChunk;
		glm::ivec3 localPos = blockPos.getLocalPos();
		return chunk->getBlock(localPos);
	}

	void World::render() {
		shader.setUniform1f(1, (float)daylight / 1800);
		chunkManager.drawChunksCubeLayers(shader);
	}


}