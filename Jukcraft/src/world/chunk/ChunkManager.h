#pragma once
#include "world/chunk/Chunk.h"
#include "blocks/Block.h"

namespace Jukcraft {
	class ChunkManager {
	public:
		ChunkManager(const std::vector<Block>& blocks);
		Shared<Chunk> createChunk(const glm::ivec2& chunkPos);
		void tick();
		void updateChunkAtPosition(Shared<Chunk>& chunk, const glm::ivec3& localPos);
		void drawChunksCubeLayers(gfx::Shader& shader);
		void drawChunksDecoLayers(gfx::Shader& shader) {}
		void drawChunksTranslucentLayers(gfx::Shader& shader) {}
		Nullable<Shared<Chunk>> getChunk(const glm::ivec2& pos);
		Nullable<Shared<const Chunk>> getChunk(const glm::ivec2& pos) const { return getChunkConst(pos); }
		Nullable<Shared<const Chunk>> getChunkConst(const glm::ivec2& pos) const;

		std::unordered_set<Shared<Chunk>>& getSkyLightPendingChunks() {
			return chunksToLight;
		}
	private:
		
		Shared<Chunk> chunks[WORLD_SIZE][WORLD_SIZE];
		std::unordered_set<Shared<Chunk>> chunksToUpdates;
		std::unordered_set<Shared<Chunk>> chunksToLight;
		const std::vector<Block>& blocks;
		gfx::Buffer chunkUbo;
		PerChunkData* mappedChunkUbo;



		friend class Chunk;
	};
}
