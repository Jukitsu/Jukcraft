#include "pch.h"
#include "world/chunk/ChunkManager.h"

namespace Jukcraft {
	ChunkManager::ChunkManager(const std::vector<Block>& blocks) :blocks(blocks) {
		std::vector<Shared<Chunk>> chunksToInitSkyLight;
		chunksToInitSkyLight.reserve(WORLD_SIZE * WORLD_SIZE);
		for (uint8_t x = 0; x < WORLD_SIZE; x++)
			for (uint8_t z = 0; z < WORLD_SIZE; z++) {
				Shared<Chunk>& chunk
					= chunks[x][z]
					= std::make_shared<Chunk>(
						glm::ivec2(x, z),
						blocks, 
						*this
					);
				chunksToUpdates.insert(chunk);
				chunksToLight.insert(chunk);
			}

		chunkUbo.allocate(sizeof(PerChunkData), nullptr, true);
		mappedChunkUbo = reinterpret_cast<PerChunkData*>(chunkUbo.map(0, sizeof(PerChunkData)));
		chunkUbo.bindRange(gfx::BufferBindingTarget::Uniform, 1, 0, sizeof(PerChunkData));
	}

	Shared<Chunk> ChunkManager::createChunk(const glm::ivec2& chunkPos) {
		int x = chunkPos.x;
		int z = chunkPos.x;
		Shared<Chunk>& chunk
			= chunks[x][z]
			= std::make_shared<Chunk>(
				glm::ivec2(x, z), 
				blocks, 
				*this
			);
		chunksToUpdates.insert(chunk);
		chunksToLight.insert(chunk);
		return chunk;
	}

	void ChunkManager::tick() {
		// std::vector<std::future<void>> results;
		for (Shared<Chunk> chunk : chunksToUpdates) {
			chunk->updateLayers();
			// results.push_back(std::async(std::launch::async, std::bind(&Chunk::updateLayers, chunk.get())));
		}
		chunksToUpdates.clear();
			
	}

	void ChunkManager::updateChunkAtPosition(Shared<Chunk>& chunk, const glm::ivec3& localPos) {
		chunksToUpdates.insert(chunk);
		const glm::ivec2& chunkPos = chunk->getChunkPos();

		for (const glm::ivec2& axis : HAXIS) {
			if (Chunk::IsOutside(localPos + glm::ivec3(axis.x, 0, axis.y))) {
				auto&& nextChunk = getChunk(chunkPos + axis);
				if (nextChunk.has_value()) {
					chunksToUpdates.insert(*nextChunk);
				}
			}
		}

	}

	Nullable<Shared<Chunk>> ChunkManager::getChunk(const glm::ivec2& chunkPos) {
		if (chunkPos.x < 0 || chunkPos.x >= WORLD_SIZE || chunkPos.y < 0 || chunkPos.y >= WORLD_SIZE)
			return {};
		return { chunks[chunkPos.x][chunkPos.y] };
	}

	Nullable<Shared<const Chunk>> ChunkManager::getChunkConst(const glm::ivec2& chunkPos) const {
		if (chunkPos.x < 0 || chunkPos.x >= WORLD_SIZE || chunkPos.y < 0 || chunkPos.y >= WORLD_SIZE)
			return {};
		return { chunks[chunkPos.x][chunkPos.y] };
	}

	void ChunkManager::drawChunksCubeLayers(gfx::Shader& shader) {
		for (uint16_t x = 0; x < WORLD_SIZE; x++)
			for (uint16_t z = 0; z < WORLD_SIZE; z++) {
				Chunk& chunk = *chunks[x][z];
				shader.bind();
				shader.setUniform3f(0, glm::vec3(x * CHUNK_DIM, 0, z * CHUNK_DIM));
				chunk.drawCubeLayer();
			}
	}
}