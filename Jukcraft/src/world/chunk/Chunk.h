#pragma once
#include "renderer/Renderer.h"
#include "renderer/chunk/Mesh.h"
#include <glm/gtc/integer.hpp>

namespace Jukcraft {

	class ChunkManager;

	class Chunk {
	public:
		Chunk(const glm::ivec2& chunkPos, const std::vector<Block>& blockTypes);
		~Chunk();
		void buildCubeLayer();
		void drawCubeLayer();
		void updateLayers();
		void uploadMesh();
		Chunk* getNeighbourChunk(const glm::ivec3& localPos);

		[[nodiscard]] constexpr const glm::ivec2& getChunkPos() { return chunkPos; }

		template<typename VectorType>
		[[nodiscard]] constexpr BlockID getBlock(const glm::vec<3, VectorType>& localPos) const {
			if (localPos.y > CHUNK_HEIGHT || localPos.x > CHUNK_DIM || localPos.z > CHUNK_DIM ||
				localPos.y < 0 || localPos.x < 0 || localPos.z < 0)
				return 0;
			return blocks[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z];
		}
		template<typename VectorType>
		void setBlock(const glm::vec<3, VectorType>& localPos, const BlockID block) {
			if (localPos.y > CHUNK_HEIGHT || localPos.x > CHUNK_DIM || localPos.z > CHUNK_DIM ||
				localPos.y < 0 || localPos.x < 0 || localPos.z < 0)
				return;
			blocks[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = block;
		}
		template<typename VectorType>
		[[nodiscard]] static constexpr glm::vec<2, VectorType> ToChunkPos(const glm::vec<3, VectorType>& worldPos) {
			return { worldPos.x / CHUNK_DIM, worldPos.z / CHUNK_DIM };
		}
		template<typename VectorType>
		[[nodiscard]] static constexpr glm::vec<3, VectorType> ToLocalPos(const glm::vec<3, VectorType>& worldPos) {
			return { glm::mod<float>((float)worldPos.x, (float)CHUNK_DIM), worldPos.y, glm::mod<float>((float)worldPos.z, (float)CHUNK_DIM) };
		}
		template<typename VectorType>
		[[nodiscard]] static constexpr glm::vec<3, VectorType> ToWorldPos(const glm::vec<2, VectorType>& chunkPos, const glm::vec<3, VectorType>& localPos) {
			return { localPos.x + chunkPos.x * CHUNK_DIM, localPos.y, localPos.z + chunkPos.y * CHUNK_DIM };
		}
		template<typename VectorType>
		[[nodiscard]] static constexpr bool IsOutside(const glm::vec<3, VectorType>& localPos) {
			return (localPos.x < 0 || localPos.x >= CHUNK_DIM
				|| localPos.y < 0 || localPos.y >= CHUNK_HEIGHT
				|| localPos.z < 0 || localPos.z >= CHUNK_DIM);
		}
		
		[[nodiscard]] constexpr bool canRenderFacing(const glm::ivec3& localPos) const {
			if (IsOutside(localPos)) {
				return getFromOutside([](std::shared_ptr<Chunk> c, const glm::ivec3& lpos)
					{
						return c->canRenderFacing(lpos);
					}, localPos);
			}
			else {
				return !getBlock(localPos);
			}
		}
		
		[[nodiscard]] constexpr bool getBlockLightSafe(const glm::ivec3& localPos) const {
			if (IsOutside(localPos)) {
				return getFromOutside([](std::shared_ptr<Chunk> c, const glm::ivec3& lpos)
					{
						return c->getBlockLightSafe(lpos);
					}, localPos);
			}
			else {
				return getBlockLight(localPos);
			}
		}

		[[nodiscard]] constexpr bool getSkyLightSafe(const glm::ivec3& localPos) const {
			if (IsOutside(localPos)) {
				return getFromOutside([](std::shared_ptr<Chunk> c, const glm::ivec3& lpos)
					{
						return c->getSkyLightSafe(lpos);
					}, localPos);
			}
			else {
				return getSkyLight(localPos);
			}
		}
		[[nodiscard]] uint8_t getFromOutside(
			std::function<uint8_t(std::shared_ptr<Chunk>, const glm::ivec3&)> f,
				const glm::ivec3& localPos) const {
			if (localPos.y >= CHUNK_HEIGHT || localPos.y < 0)
				return 15;
			else if (localPos.z <= -1 && !neighbourChunks.north.expired()) {
				return f(neighbourChunks.north.lock(), glm::ivec3 { localPos.x, localPos.y, CHUNK_DIM - localPos.z });
			}
			else if (localPos.z >= CHUNK_DIM && !neighbourChunks.south.expired()) {
				return f(neighbourChunks.south.lock(), glm::ivec3 { localPos.x, localPos.y, localPos.z - CHUNK_DIM });
			}
			else if (localPos.x <= -1 && !neighbourChunks.west.expired()) {
				return f(neighbourChunks.west.lock(), glm::ivec3 { CHUNK_DIM - localPos.x, localPos.y, localPos.z });
			}
			else if (localPos.x >= CHUNK_DIM && !neighbourChunks.east.expired()) {
				return f(neighbourChunks.east.lock(), glm::ivec3 { localPos.x - CHUNK_DIM, localPos.y, localPos.z });
			}
			else
				return 15;
		}
		template<typename VectorType>
		[[nodiscard]] constexpr uint8_t getRawLight(const glm::vec<3, VectorType>& localPos) const noexcept {
			return lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z];
		}
		template<typename VectorType>
		[[nodiscard]] constexpr uint8_t getBlockLight(const glm::vec<3, VectorType>& localPos) const noexcept {
			return lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF;
		}
		template<typename VectorType>
		[[nodiscard]] constexpr uint8_t getSkyLight(const glm::vec<3, VectorType>& localPos) const noexcept {
			return (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] >> 4) & 0xF;
		}
		template<typename VectorType>
		void setBlockLight(const glm::vec<3, VectorType>& localPos, uint8_t value) noexcept {
			lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF0) | value;
		}
		template<typename VectorType>
		void setSkyLight(const glm::vec<3, VectorType>& localPos, uint8_t value) noexcept {
			lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF) | (value << 4);
		}
	private:
		struct {
			std::weak_ptr<Chunk> east;
			std::weak_ptr<Chunk> west;
			std::weak_ptr<Chunk> south;
			std::weak_ptr<Chunk> north;
		} neighbourChunks;

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
}