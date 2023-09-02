#include "pch.h"
#include "world/World.h"

World::World(const std::vector<Block>& blocks, Shader& shader)
	:chunkManager(blocks), shader(shader), blocks(blocks) {

}

void World::tick() {
	chunkManager.tick();
	propagateLightIncrease();
}

void World::setBlock(const glm::ivec3& worldPos, BlockID block) {
	glm::ivec2 chunkPos = Chunk::ToChunkPos(worldPos);
	std::optional<std::shared_ptr<Chunk>> pendingChunk = chunkManager.getChunk(chunkPos);
	if (!pendingChunk.has_value())
		return;
	std::shared_ptr<Chunk>& chunk = *pendingChunk;
	glm::ivec3 localPos = Chunk::ToLocalPos(worldPos);
	if (block == 5) {
		increaseLight(worldPos, chunk);
	}
	chunk->setBlock(localPos, block);
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
	chunkManager.drawChunksCubeLayers(shader);
}

void World::increaseLight(const glm::ivec3& pos, std::shared_ptr<Chunk>& chunk, uint8_t light) {
	chunk->setBlockLight(Chunk::ToLocalPos(pos), light);

	lightIncreaseQueue.push(
		BlockLightIncreaseNode{
			pos,
			light
		}
	);
	propagateLightIncrease();
}

void World::propagateLightIncrease() {
	while (!lightIncreaseQueue.empty()) {
		const BlockLightIncreaseNode& node = lightIncreaseQueue.front();
		glm::ivec3 pos = node.pos;
		uint8_t light = node.light;
		lightIncreaseQueue.pop();
		

		for (const glm::ivec3& direction : IDIRECTIONS) {
			glm::ivec3 newPos = pos + direction;
			std::optional<std::shared_ptr<Chunk>> chunk = chunkManager.getChunk(Chunk::ToChunkPos(newPos));
			if (!chunk.has_value())
				continue;
			glm::ivec3 localPos = Chunk::ToLocalPos(newPos);
			if (chunk.value()->getBlockLight(localPos) + 2 <= light && blocks[chunk.value()->getBlock(localPos)].isTransparent()) {
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

void World::propagateLightDecrease() {

}
