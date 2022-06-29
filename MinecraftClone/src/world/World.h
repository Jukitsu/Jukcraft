#pragma once
#include "world/chunk/ChunkManager.h"

class World {
public:
	World(const std::vector<Block>& blocks, Shader& shader);
	void tick();
	void render();
	void setBlock(const glm::ivec3& worldPos, BlockID block);
	BlockID World::getBlock(const glm::ivec3& worldPos) const;
private:
	Shader& shader;
	ChunkManager chunkManager;
};