#pragma once
#include "world/chunk/ChunkManager.h"

class World {
public:
	World(const std::vector<Block>& blocks, Shader& shader);
	void tick();
	void render();
	void setBlock(const glm::ivec3& worldPos, BlockID block);
	
	BlockID getBlock(const glm::ivec3& worldPos) const;
	void increaseLight(const glm::ivec3& pos, std::shared_ptr<Chunk>& chunk, uint8_t light = 14);
	void propagateLightIncrease();
	void propagateLightDecrease();


private:
	struct BlockLightIncreaseNode {
		glm::ivec3 pos;
		uint8_t light;
	};
	struct BlockLightDecreaseNode {
		uint16_t index;
		uint8_t light;
	};
	struct SkyLightIncreaseNode {
		uint16_t index;
		uint8_t light;
	};
	struct SkyLightDecreaseNode {
		uint16_t index;
		uint8_t light;
	};
	const std::vector<Block>& blocks;
	Shader& shader;
	ChunkManager chunkManager;
	std::queue<BlockLightIncreaseNode> lightIncreaseQueue;
	std::queue<SkyLightIncreaseNode> skylightIncreaseQueue;
	std::queue<BlockLightDecreaseNode> lightDecreaseQueue;
	std::queue<SkyLightDecreaseNode> skylightDecreaseQueue;
};