#include "pch.h"
#include "world/World.h"

World::World(const std::vector<Block>& blocks, Shader& shader)
	:chunkManager(blocks), shader(shader) {

}

void World::tick() {
	chunkManager.tick();
}

void World::setBlock(const glm::ivec3& worldPos, BlockID block) {
	glm::ivec2 chunkPos = Chunk::ToChunkPos(worldPos);
	std::optional<std::shared_ptr<Chunk>> pendingChunk = chunkManager.getChunk(chunkPos);
	if (!pendingChunk.has_value())
		return;
	std::shared_ptr<Chunk>& chunk = *pendingChunk;
	glm::ivec3 localPos = Chunk::ToLocalPos(worldPos);
	chunk->setBlock(localPos, block);
	chunk->updateAtPosition(localPos);
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
	chunkManager.drawChunksCubeLayers(shader);
}
