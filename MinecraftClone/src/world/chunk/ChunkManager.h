#pragma once
#include "world/chunk/Chunk.h"
#include "blocks/Block.h"

class ChunkManager {
public:
	ChunkManager(const std::vector<Block>& blocks);
	void tick();
	void drawChunksCubeLayers(Shader& shader);
	void drawChunksDecoLayers(Shader& shader) {}
	void drawChunksTranslucentLayers(Shader& shader) {}
	std::optional<std::shared_ptr<Chunk>> getChunk(const glm::ivec2& pos);
	std::optional<std::shared_ptr<const Chunk>> getChunk(const glm::ivec2& pos)const ;
private:
	std::shared_ptr<Chunk> chunks[WORLD_SIZE][WORLD_SIZE];
	std::queue<std::shared_ptr<Chunk>> chunkBuildingQueue;
	Buffer chunkUbo;
	PerChunkData* mappedChunkUbo;
};
