#pragma once
#include "world/chunk/ChunkManager.h"
#include "world/LightEngine.h"

class World {
public:
	World(const std::vector<Block>& blocks, Shader& shader);
	void tick();
	void render();
	void setBlock(const glm::ivec3& worldPos, BlockID block);
	
	BlockID getBlock(const glm::ivec3& worldPos) const;
private:
	const std::vector<Block>& blocks;
	Shader& shader;
	ChunkManager chunkManager;
	LightEngine lightEngine;
};