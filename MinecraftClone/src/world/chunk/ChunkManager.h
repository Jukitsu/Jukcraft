#include "world/chunk/Chunk.h"
class ChunkManager {
public:
	ChunkManager();
	void tick();
	void drawChunksCubeLayers(Shader& shader);
	void drawChunksDecoLayers(Shader& shader) {}
	void drawChunksTranslucentLayers(Shader& shader) {}
private:
	std::shared_ptr<Chunk> chunks[WORLD_SIZE][WORLD_SIZE];
	std::queue<std::shared_ptr<Chunk>> chunkBuildingQueue;
};
