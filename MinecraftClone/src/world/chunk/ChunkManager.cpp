#include "pch.h"
#include "world/chunk/ChunkManager.h"

ChunkManager::ChunkManager() {
	for (uint8_t x = 0; x < WORLD_SIZE; x++)
		for (uint8_t z = 0; z < WORLD_SIZE; z++) {
			std::shared_ptr<Chunk>& chunk
				= chunks[x][z]
				= std::make_shared<Chunk>(glm::ivec2(x, z));
			for (uint8_t ly = 0; ly < CHUNK_HEIGHT / 2; ly++) 
				for (uint8_t lx = 0; lx < CHUNK_DIM; lx++)
					for (uint8_t lz = 0; lz < CHUNK_DIM; lz++) {
						chunk->setBlock(glm::uvec3(lx, ly, lz), !(std::rand() % 5));
					}
			chunkBuildingQueue.push(chunk);
			if (z > 0) {
				std::shared_ptr<Chunk>& northernChunk = chunks[x][z - 1];
				northernChunk->neighbourChunks.south = chunk;
				chunk->neighbourChunks.north = northernChunk;
			}
			if (x > 0) {
				std::shared_ptr<Chunk>& westernChunk = chunks[x - 1][z];
				westernChunk->neighbourChunks.east = chunk;
				chunk->neighbourChunks.west = westernChunk;
			}

		}
}

void ChunkManager::tick() {
	if (!chunkBuildingQueue.empty()) {
		std::shared_ptr<Chunk>& chunk = chunkBuildingQueue.front();
		chunk->buildCubeLayer();
		chunkBuildingQueue.pop();
	}
}

void ChunkManager::drawChunksCubeLayers(Shader& shader) {
	for (uint16_t x = 0; x < WORLD_SIZE; x++)
		for (uint16_t z = 0; z < WORLD_SIZE; z++) {
			Chunk& chunk = *chunks[x][z];
			shader.setUniform3f(0, glm::vec3(x * CHUNK_DIM, 0, z * CHUNK_DIM));
			chunk.drawCubeLayer();
		}
}