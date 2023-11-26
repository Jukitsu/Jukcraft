#include "pch.h"
#include "world/chunk/ChunkManager.h"

namespace Jukcraft {
	ChunkManager::ChunkManager(const std::vector<Block>& blocks) :blocks(blocks) {
		std::vector<std::shared_ptr<Chunk>> chunksToInitSkyLight;
		chunksToInitSkyLight.reserve(WORLD_SIZE * WORLD_SIZE);
		for (uint8_t x = 0; x < WORLD_SIZE; x++)
			for (uint8_t z = 0; z < WORLD_SIZE; z++) {
				std::shared_ptr<Chunk>& chunk
					= chunks[x][z]
					= std::make_shared<Chunk>(glm::ivec2(x, z), blocks);
				chunksToUpdates.insert(chunk);
				chunksToLight.insert(chunk);
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

		chunkUbo.allocate(sizeof(PerChunkData), nullptr, true);
		mappedChunkUbo = reinterpret_cast<PerChunkData*>(chunkUbo.map(0, sizeof(PerChunkData)));
		chunkUbo.bindRange(gfx::BufferBindingTarget::Uniform, 1, 0, sizeof(PerChunkData));
	}

	std::shared_ptr<Chunk> ChunkManager::createChunk(const glm::ivec2& chunkPos) {
		int x = chunkPos.x;
		int z = chunkPos.x;
		std::shared_ptr<Chunk>& chunk
			= chunks[x][z]
			= std::make_shared<Chunk>(glm::ivec2(x, z), blocks);
		chunksToUpdates.insert(chunk);
		chunksToLight.insert(chunk);
		return chunk;
	}

	void ChunkManager::tick() {
		std::vector<std::future<void>> results;
		for (std::shared_ptr<Chunk> chunk : chunksToUpdates) {
			chunk->updateLayers();
			// results.push_back(std::async(std::launch::async, std::bind(&Chunk::updateLayers, chunk.get())));
		}
		for (std::shared_ptr<Chunk> chunk : chunksToUpdates) {
			chunk->uploadMesh();
		}
		chunksToUpdates.clear();
	}

	void ChunkManager::updateChunkAtPosition(std::shared_ptr<Chunk>& chunk, const glm::ivec3& localPos) {
		chunksToUpdates.insert(chunk);

		if (Chunk::IsOutside(localPos + IEAST) && !chunk->neighbourChunks.east.expired())
			chunksToUpdates.insert(chunk->neighbourChunks.east.lock());
		if (Chunk::IsOutside(localPos + INORTH) && !chunk->neighbourChunks.north.expired())
			chunksToUpdates.insert(chunk->neighbourChunks.north.lock());
		if (Chunk::IsOutside(localPos + IWEST) && !chunk->neighbourChunks.west.expired())
			chunksToUpdates.insert(chunk->neighbourChunks.west.lock());
		if (Chunk::IsOutside(localPos + ISOUTH) && !chunk->neighbourChunks.south.expired())
			chunksToUpdates.insert(chunk->neighbourChunks.south.lock());

	}

	std::optional<std::shared_ptr<Chunk>> ChunkManager::getChunk(const glm::ivec2& chunkPos) {
		if (chunkPos.x < 0 || chunkPos.x >= WORLD_SIZE || chunkPos.y < 0 || chunkPos.y >= WORLD_SIZE)
			return {};
		return { chunks[chunkPos.x][chunkPos.y] };
	}

	std::optional<std::shared_ptr<const Chunk>> ChunkManager::getChunk(const glm::ivec2& chunkPos) const {
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