#pragma once
#include "renderer/Renderer.h"
#include "renderer/chunk/Mesh.h"
#include <glm/gtc/integer.hpp>

namespace Jukcraft {

	class Chunk;

	using ChunkGetter = std::function<std::optional<std::shared_ptr<const Chunk>>(const glm::ivec2&)>;
	

	class Chunk {
	public:
		Chunk(const glm::ivec2& chunkPos, const std::vector<Block>& blockTypes, 
			const ChunkGetter& chunkGetter);
		~Chunk();
		void buildCubeLayer();
		void drawCubeLayer();
		void updateLayers();
		void uploadMesh();
		Chunk* getNeighbourChunk(const glm::ivec3& localPos);

		[[nodiscard]] constexpr const glm::ivec2& getChunkPos() { return chunkPos; }

		template<typename VectorType>
		[[nodiscard]] constexpr BlockID getBlock(const glm::vec<3, VectorType>& localPos) const;
		template<typename VectorType>
		void setBlock(const glm::vec<3, VectorType>& localPos, const BlockID block);

		template<typename VectorType>
		[[nodiscard]] static constexpr glm::vec<2, VectorType> ToChunkPos(const glm::vec<3, VectorType>& worldPos);
		template<typename VectorType>
		[[nodiscard]] static constexpr glm::vec<3, VectorType> ToLocalPos(const glm::vec<3, VectorType>& worldPos);
		template<typename VectorType>
		[[nodiscard]] static constexpr glm::vec<3, VectorType> ToWorldPos(const glm::vec<2, VectorType>& chunkPos, const glm::vec<3, VectorType>& localPos);
		template<typename VectorType>
		[[nodiscard]] static constexpr bool IsOutside(const glm::vec<3, VectorType>& localPos);
		
		[[nodiscard]] bool canRenderFacing(const glm::ivec3& localPos) const; 
		[[nodiscard]] uint8_t getBlockLightSafe(const glm::ivec3& localPos) const;
		[[nodiscard]] uint8_t getSkyLightSafe(const glm::ivec3& localPos) const;


		template<typename VectorType>
		[[nodiscard]] constexpr uint8_t getRawLight(const glm::vec<3, VectorType>& localPos) const noexcept;
		template<typename VectorType>
		[[nodiscard]] constexpr uint8_t getBlockLight(const glm::vec<3, VectorType>& localPos) const noexcept;
		template<typename VectorType>
		[[nodiscard]] constexpr uint8_t getSkyLight(const glm::vec<3, VectorType>& localPos) const noexcept;
		template<typename VectorType>
		void setBlockLight(const glm::vec<3, VectorType>& localPos, uint8_t value) noexcept;
		template<typename VectorType>
		void setSkyLight(const glm::vec<3, VectorType>& localPos, uint8_t value) noexcept;
	private:
		struct {
			std::weak_ptr<Chunk> east;
			std::weak_ptr<Chunk> west;
			std::weak_ptr<Chunk> south;
			std::weak_ptr<Chunk> north;
		} neighbourChunks;

		ChunkGetter chunkGetter;

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

	template<typename VectorType>
	[[nodiscard]] constexpr uint8_t Chunk::getRawLight(const glm::vec<3, VectorType>& localPos) const noexcept {
		return lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z];
	}
	template<typename VectorType>
	[[nodiscard]] constexpr uint8_t Chunk::getBlockLight(const glm::vec<3, VectorType>& localPos) const noexcept {
		return lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF;
	}
	template<typename VectorType>
	[[nodiscard]] constexpr uint8_t Chunk::getSkyLight(const glm::vec<3, VectorType>& localPos) const noexcept {
		return (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] >> 4) & 0xF;
	}
	template<typename VectorType>
	inline void Chunk::setBlockLight(const glm::vec<3, VectorType>& localPos, uint8_t value) noexcept {
		lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF0) | value;
	}
	template<typename VectorType>
	inline void Chunk::setSkyLight(const glm::vec<3, VectorType>& localPos, uint8_t value) noexcept {
		lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = (lightMap[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] & 0xF) | (value << 4);
	}
	template<typename VectorType>
	[[nodiscard]] constexpr BlockID Chunk::getBlock(const glm::vec<3, VectorType>& localPos) const {
		if (localPos.y > CHUNK_HEIGHT || localPos.x > CHUNK_DIM || localPos.z > CHUNK_DIM ||
			localPos.y < 0 || localPos.x < 0 || localPos.z < 0)
			return 0;
		return blocks[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z];
	}
	template<typename VectorType>
	inline void Chunk::setBlock(const glm::vec<3, VectorType>& localPos, const BlockID block) {
		if (localPos.y > CHUNK_HEIGHT || localPos.x > CHUNK_DIM || localPos.z > CHUNK_DIM ||
			localPos.y < 0 || localPos.x < 0 || localPos.z < 0)
			return;
		blocks[(uint8_t)localPos.y][(uint8_t)localPos.x][(uint8_t)localPos.z] = block;
	}
	template<typename VectorType>
	[[nodiscard]] static constexpr glm::vec<2, VectorType> Chunk::ToChunkPos(const glm::vec<3, VectorType>& worldPos) {
		return { glm::floor((float)worldPos.x / CHUNK_DIM), glm::floor((float)worldPos.z / CHUNK_DIM) };
	}
	template<typename VectorType>
	[[nodiscard]] static constexpr glm::vec<3, VectorType> Chunk::ToLocalPos(const glm::vec<3, VectorType>& worldPos) {
		return { glm::mod<float>((float)worldPos.x, (float)CHUNK_DIM), worldPos.y, glm::mod<float>((float)worldPos.z, (float)CHUNK_DIM) };
	}
	template<typename VectorType>
	[[nodiscard]] static constexpr glm::vec<3, VectorType> Chunk::ToWorldPos(const glm::vec<2, VectorType>& chunkPos, const glm::vec<3, VectorType>& localPos) {
		return { localPos.x + chunkPos.x * CHUNK_DIM, localPos.y, localPos.z + chunkPos.y * CHUNK_DIM };
	}
	template<typename VectorType>
	[[nodiscard]] static constexpr bool Chunk::IsOutside(const glm::vec<3, VectorType>& localPos) {
		return (localPos.x < 0 || localPos.x >= CHUNK_DIM
			|| localPos.y < 0 || localPos.y >= CHUNK_HEIGHT
			|| localPos.z < 0 || localPos.z >= CHUNK_DIM);
	}
}