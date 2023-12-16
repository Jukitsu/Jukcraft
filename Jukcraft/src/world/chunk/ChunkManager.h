#pragma once
#include "world/chunk/Chunk.h"
#include "blocks/Block.h"

namespace Jukcraft {
	class ChunkManager {
	public:
		ChunkManager(const std::vector<Block>& blocks);
		std::shared_ptr<Chunk> createChunk(const glm::ivec2& chunkPos);
		void tick();
		void updateChunkAtPosition(std::shared_ptr<Chunk>& chunk, const glm::ivec3& localPos);
		void drawChunksCubeLayers(gfx::Shader& shader);
		void drawChunksDecoLayers(gfx::Shader& shader) {}
		void drawChunksTranslucentLayers(gfx::Shader& shader) {}
		std::optional<std::shared_ptr<Chunk>> getChunk(const glm::ivec2& pos);
		std::optional<std::shared_ptr<const Chunk>> getChunk(const glm::ivec2& pos) const { return getChunkConst(pos); }
		std::optional<std::shared_ptr<const Chunk>> getChunkConst(const glm::ivec2& pos) const;

		std::unordered_set<std::shared_ptr<Chunk>>& getSkyLightPendingChunks() {
			return chunksToLight;
		}
	private:
		
		std::shared_ptr<Chunk> chunks[WORLD_SIZE][WORLD_SIZE];
		std::queue<std::shared_ptr<Chunk>> chunksToUpdates;
		std::unordered_set<std::shared_ptr<Chunk>> chunksToLight;
		const std::vector<Block>& blocks;
		gfx::Buffer chunkUbo;
		PerChunkData* mappedChunkUbo;



		friend class Chunk;
	};
}
