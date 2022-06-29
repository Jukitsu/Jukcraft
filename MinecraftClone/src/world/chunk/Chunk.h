#pragma once
#include "renderer/Renderer.h"
#include "blocks/Block.h"
#include <glm/gtc/integer.hpp>

class ChunkManager;

class Chunk {
public:
	Chunk(const glm::ivec2& chunkPos, const std::vector<Block>& blockTypes);
	~Chunk();
	void buildCubeLayer();
	void drawCubeLayer();

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
		return { glm::mod<float>(worldPos.x, CHUNK_DIM), worldPos.y, glm::mod<float>(worldPos.z, CHUNK_DIM) };
	}
	template<typename VectorType>
	[[nodiscard]] static constexpr bool IsOutside(const glm::vec<3, VectorType>& localPos) {
		return (localPos.x < 0 || localPos.x >= CHUNK_DIM
			 || localPos.y < 0 || localPos.y >= CHUNK_HEIGHT
			 || localPos.z < 0 || localPos.z >= CHUNK_DIM);
	}
	template<typename VectorType> 
	[[nodiscard]] constexpr bool canRenderFacing(const glm::vec<3, VectorType>& localPos) const {
		if (IsOutside(localPos)) {
			return canRenderFacingOutside(localPos);
		}
		else {
			return !getBlock(localPos);
		}
	}
	template<typename VectorType>
	[[nodiscard]] constexpr bool canRenderFacingOutside(const glm::vec<3, VectorType>& localPos) const {
		if (localPos.z == -1 && !neighbourChunks.north.expired()) {
			return !neighbourChunks.north.lock()->getBlock(glm::ivec3{ localPos.x, localPos.y, CHUNK_DIM - 1 });
		}
		else if (localPos.z == CHUNK_DIM && !neighbourChunks.south.expired()) {
			return !neighbourChunks.south.lock()->getBlock(glm::ivec3{ localPos.x, localPos.y, 0 });
		}
		else if (localPos.x == -1 && !neighbourChunks.west.expired()) {
			return !neighbourChunks.west.lock()->getBlock(glm::ivec3{ CHUNK_DIM - 1, localPos.y, localPos.z });
		}
		else if (localPos.x == CHUNK_DIM && !neighbourChunks.east.expired()) {
			return !neighbourChunks.east.lock()->getBlock(glm::ivec3{ 0, localPos.y, localPos.z });
		}
		else 
			return true;
	}
	void updateAtPosition(const glm::vec3& localPos);
	void updateLayers();
private:
	void pushQuad(const Quad& quad, const glm::uvec3& localPos, uint8_t textureID);
	struct {
		std::weak_ptr<Chunk> east;
		std::weak_ptr<Chunk> west;
		std::weak_ptr<Chunk> south;
		std::weak_ptr<Chunk> north;
	} neighbourChunks;
	BlockID*** blocks;
	const std::vector<Block>& blockTypes;
	VertexArray vao;
	DynamicBuffer<uint32_t> vbo;
	DynamicBuffer<DrawIndirectCommand> icbo;
	std::vector<uint32_t> vertices;
	std::vector<uint32_t> indices;
	bool drawable = false;

	FORBID_COPY(Chunk);
	FORBID_MOVE(Chunk);

	friend class ChunkManager;
};