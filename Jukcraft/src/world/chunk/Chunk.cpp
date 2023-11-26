#include "pch.h"
#include "renderer/Renderer.h"
#include "world/chunk/Chunk.h"


namespace Jukcraft {


	Chunk::Chunk(const glm::ivec2& chunkPos, const std::vector<Block>& blockTypes)
		:blockTypes(blockTypes), chunkPos(chunkPos), mesh(CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 6 * 4)
	{
		blocks = new BlockID * *[CHUNK_HEIGHT];
		for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
			blocks[j] = new BlockID * [CHUNK_DIM];
			for (size_t i = 0; i < CHUNK_DIM; i++) {
				blocks[j][i] = new BlockID[CHUNK_DIM];
				memset(blocks[j][i], 0, CHUNK_DIM * sizeof(BlockID));
			}
		}

		for (uint8_t ly = 0; ly < CHUNK_HEIGHT / 2; ly++)
			for (uint8_t lx = 0; lx < CHUNK_DIM; lx++)
				for (uint8_t lz = 0; lz < CHUNK_DIM; lz++) {
					if (ly == CHUNK_HEIGHT / 2 - 1)
						setBlock(glm::uvec3(lx, ly, lz), 2);
					else if (ly >= CHUNK_HEIGHT / 2 - 3 && ly < CHUNK_HEIGHT / 2 - 1)
						setBlock(glm::uvec3(lx, ly, lz), 3);
					else
						setBlock(glm::uvec3(lx, ly, lz), 1);
				}

		lightMap = new uint8_t * *[CHUNK_HEIGHT];
		for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
			lightMap[j] = new uint8_t * [CHUNK_DIM];
			for (size_t i = 0; i < CHUNK_DIM; i++) {
				lightMap[j][i] = new uint8_t[CHUNK_DIM];
				memset(lightMap[j][i], 0, CHUNK_DIM * sizeof(uint8_t));
			}
		}

	}

	Chunk::~Chunk() {
		for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
			for (size_t i = 0; i < CHUNK_DIM; i++)
				delete[] blocks[j][i];
			delete[] blocks[j];
		}
		delete[] blocks;

		for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
			for (size_t i = 0; i < CHUNK_DIM; i++)
				delete[] lightMap[j][i];
			delete[] lightMap[j];
		}
		delete[] lightMap;

		if (!neighbourChunks.east.expired())
			neighbourChunks.east.lock()->neighbourChunks.west.reset();
		if (!neighbourChunks.west.expired())
			neighbourChunks.west.lock()->neighbourChunks.east.reset();
		if (!neighbourChunks.south.expired())
			neighbourChunks.south.lock()->neighbourChunks.north.reset();
		if (!neighbourChunks.north.expired())
			neighbourChunks.north.lock()->neighbourChunks.south.reset();
	}



	void Chunk::buildCubeLayer() {
		mesh.begin();
		uint32_t quadCount = 0;
		for (uint8_t y = 0; y < CHUNK_HEIGHT; y++)
			for (uint8_t x = 0; x < CHUNK_DIM; x++)
				for (uint8_t z = 0; z < CHUNK_DIM; z++) {
					glm::ivec3 localPos = glm::ivec3(x, y, z);

					BlockID block = getBlock(localPos);

					if (!block)
						continue;

					Block type = blockTypes[block];
					if (true) {
						for (uint8_t i = 0; i < 6; i++) {
							if (canRenderFacing(localPos + IDIRECTIONS[i])) {
								uint8_t light = 15 << 4;
								Chunk* neighbourChunk = getNeighbourChunk(localPos + IDIRECTIONS[i]);
								if (neighbourChunk)
									light = neighbourChunk->getRawLight(ToLocalPos(localPos + IDIRECTIONS[i]));
								mesh.pushQuad(type.getModel().getQuads()[i], localPos, type.getTextureLayout()[i], light);
								quadCount++;
							}
						}
					}

				}
		mesh.end();

		
		drawable = true;
	}

	Chunk* Chunk::getNeighbourChunk(const glm::ivec3& localPos) {
		if (!IsOutside(localPos))
			return this;
		if (localPos.x >= CHUNK_DIM && !neighbourChunks.east.expired())
			return neighbourChunks.east.lock().get();
		if (localPos.z < 0 && !neighbourChunks.north.expired())
			return neighbourChunks.north.lock().get();
		if (localPos.x < 0 && !neighbourChunks.west.expired())
			return neighbourChunks.west.lock().get();
		if (localPos.z >= CHUNK_DIM && !neighbourChunks.south.expired())
			return neighbourChunks.south.lock().get();
		return nullptr;
	}



	void Chunk::updateLayers() {
		buildCubeLayer();
	}

	void Chunk::uploadMesh() {
		
	}
	void Chunk::drawCubeLayer() {
		if (!drawable)
			return;
		mesh.draw();
	}
}