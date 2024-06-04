#include "pch.h"
#include "renderer/Renderer.h"
#include "world/chunk/Chunk.h"

#include "world/chunk/ChunkManager.h"

namespace Jukcraft {

	Chunk::Chunk(const glm::ivec2& chunkPos, const std::vector<Block>& blockTypes, ChunkManager& chunkManager)
		:blockTypes(blockTypes), chunkPos(chunkPos), chunkManager(chunkManager),
		mesh(
			CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 6 * 4, 
			*this		
		)
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
						setBlock(glm::uvec3(lx, ly, lz), std::rand() % 3 ? 1 : 0);
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
			
					const Block& type = blockTypes[block];
					
					for (uint8_t i = 0; i < 6; i++) {
						if (canRenderFacing(localPos + IDIRECTIONS[i])) {
							uint8_t blocklight = getBlockLightSafe(localPos + IDIRECTIONS[i]);
							uint8_t skylight = getSkyLightSafe(localPos + IDIRECTIONS[i]);

							BakedQuad bakedQuad = mesh.bakeCubeFace(localPos, i, blocklight, skylight);
							mesh.pushCubeFace(type.getModel().getQuads()[i], localPos, type.getTextureLayout()[i], bakedQuad);
							quadCount++;
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


	[[nodiscard]] uint8_t Chunk::getOpacitySafe(const glm::ivec3& localPos) const {
		if (IsOutside(localPos)) {
			BlockPos blockPos(chunkPos, localPos);
			glm::ivec2 newChunkPos = blockPos.getChunkPos();
			glm::ivec3 newLocalPos = blockPos.getLocalPos();
			std::optional<std::shared_ptr<const Chunk>> chunk = chunkManager.getChunk(newChunkPos);
			if (!chunk.has_value() || IsOutside(newLocalPos))
				return 0;
			return blockTypes[(*chunk)->getBlock(newLocalPos)].getOpacity();
		}
		else {
			return blockTypes[getBlock(localPos)].getOpacity();
		}
	}

	[[nodiscard]] uint8_t Chunk::getBlockLightSafe(const glm::ivec3& localPos) const {
		if (IsOutside(localPos)) {
			BlockPos blockPos(chunkPos, localPos);
			glm::ivec2 newChunkPos = blockPos.getChunkPos();
			glm::ivec3 newLocalPos = blockPos.getLocalPos();
			std::optional<std::shared_ptr<const Chunk>> chunk = chunkManager.getChunk(newChunkPos);
			if (!chunk.has_value() || IsOutside(newLocalPos))
				return 0;
			return (*chunk)->getBlockLight(newLocalPos);
		}
		else {
			return getBlockLight(localPos);
		}
	}

	[[nodiscard]] uint8_t Chunk::getSkyLightSafe(const glm::ivec3& localPos) const {
		if (IsOutside(localPos)) {
			BlockPos blockPos(chunkPos, localPos);
			glm::ivec2 newChunkPos = blockPos.getChunkPos();
			glm::ivec3 newLocalPos = blockPos.getLocalPos();
			std::optional<std::shared_ptr<const Chunk>> chunk = chunkManager.getChunk(newChunkPos);
			if (!chunk.has_value() || IsOutside(newLocalPos))
				return 15;
			return (*chunk)->getSkyLight(newLocalPos);
		}
		else {
			return getSkyLight(localPos);
		}
	}

	
	void Chunk::updateLayers() {
		buildCubeLayer();
	}

	void Chunk::uploadMesh() {
		// For future use
	}
	void Chunk::drawCubeLayer() {
		if (!drawable)
			return;
		mesh.draw();
	}
}