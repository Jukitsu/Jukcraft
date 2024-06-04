#pragma once
#include "renderer/Renderer.h"
#include "renderer/chunk/Mesh.h"
#include <glm/gtc/integer.hpp>

namespace Jukcraft {

	class ChunkManager;
	

	class Chunk {
	public:
		Chunk(const glm::ivec2& chunkPos, const std::vector<Block>& blockTypes, ChunkManager& chunkGetter);
		~Chunk();
		void buildCubeLayer();
		void drawCubeLayer();
		void updateLayers();
		void uploadMesh();
		Chunk* getNeighbourChunk(const glm::ivec3& localPos);

		[[nodiscard]] constexpr const glm::ivec2& getChunkPos() { return chunkPos; }

		[[nodiscard]] constexpr BlockID getBlock(const glm::ivec3& localPos) const;

		void setBlock(const glm::ivec3& localPos, const BlockID block);

		[[nodiscard]] static constexpr bool IsOutside(const glm::ivec3& localPos);
		
		[[nodiscard]] bool canRenderFacing(const glm::ivec3& localPos) const { return !getOpacitySafe(localPos); }
		[[nodiscard]] uint8_t getOpacitySafe(const glm::ivec3& localPos) const;
		[[nodiscard]] uint8_t getBlockLightSafe(const glm::ivec3& localPos) const;
		[[nodiscard]] uint8_t getSkyLightSafe(const glm::ivec3& localPos) const;


		[[nodiscard]] constexpr uint8_t getRawLight(const glm::ivec3& localPos) const noexcept {
			return lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z];
		}

		[[nodiscard]] constexpr uint8_t getBlockLight(const glm::ivec3& localPos) const noexcept {
			return lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF;
		}

		[[nodiscard]] constexpr uint8_t getSkyLight(const glm::ivec3& localPos) const noexcept {
			return (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] >> 4) & 0xF;
		}

		void setBlockLight(const glm::ivec3& localPos, uint8_t value) noexcept {
			lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF0) | value;
		}

		void setSkyLight(const glm::ivec3& localPos, uint8_t value) noexcept {
			lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF) | (value << 4);
		}
	private:
		struct {
			std::weak_ptr<Chunk> east;
			std::weak_ptr<Chunk> west;
			std::weak_ptr<Chunk> south;
			std::weak_ptr<Chunk> north;
		} neighbourChunks;

		ChunkManager& chunkManager;

		BlockID*** blocks;

		uint8_t*** lightMap;

		Mesh mesh;

		const std::vector<Block>& blockTypes;

		bool drawable = false;
		glm::ivec2 chunkPos;

		FORBID_COPY(Chunk);
		FORBID_MOVE(Chunk);

		friend class ChunkManager;
	};

	

	[[nodiscard]] constexpr BlockID Chunk::getBlock(const glm::ivec3& localPos) const {
		if (localPos.y > CHUNK_HEIGHT || localPos.x > CHUNK_DIM || localPos.z > CHUNK_DIM ||
			localPos.y < 0 || localPos.x < 0 || localPos.z < 0)
			return 0;
		return blocks[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z];
	}
	inline void Chunk::setBlock(const glm::ivec3& localPos, const BlockID block) {
		if (localPos.y > CHUNK_HEIGHT || localPos.x > CHUNK_DIM || localPos.z > CHUNK_DIM ||
			localPos.y < 0 || localPos.x < 0 || localPos.z < 0)
			return;
		blocks[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = block;
	}

	[[nodiscard]] constexpr bool Chunk::IsOutside(const glm::ivec3& localPos) {
		return (localPos.x < 0 || localPos.x >= CHUNK_DIM
			|| localPos.y < 0 || localPos.y >= CHUNK_HEIGHT
			|| localPos.z < 0 || localPos.z >= CHUNK_DIM);
	}
}