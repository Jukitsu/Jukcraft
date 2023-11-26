#pragma once
#include "renderer/gfx/buffers/DynamicBuffer.h"
#include "blocks/Block.h"

namespace Jukcraft {

	struct BakedQuad {
		std::array<uint8_t, 4> smoothBlockLightData;
		std::array<uint8_t, 4> smoothSkyLightData;
		std::array<uint8_t, 4> ambientOcclusionData;
	};

	struct MeshDelegates {
		std::function<bool(const glm::ivec3&)> opacityGetterDelegate;
		std::function<uint8_t(const glm::ivec3&)> lightGetterDelegate;
		std::function<uint8_t(const glm::ivec3&)> skylightGetterDelegate;
	};

	using OpacityCheckDelegate = std::function<bool(const glm::ivec3&)>;

	class Mesh {
	public:
		Mesh(size_t size, const MeshDelegates& meshDelegates);
		~Mesh();
		void begin();
		BakedQuad bakeCubeFace(const glm::ivec3& localPos, uint8_t normalIndex, uint8_t blocklight, uint8_t skylight);
		void pushCubeFace(const Quad& quad, const glm::uvec3& localPos, uint8_t textureID, const BakedQuad& bakedQuad);
		void end();
		void draw();
	private:
		std::array<uint8_t, 4> getFaceAO(uint8_t s1, uint8_t s2, uint8_t s3,
										 uint8_t s4,			 uint8_t s5,
										 uint8_t s6, uint8_t s7, uint8_t s8);
		std::array<uint8_t, 4> getSmoothLighting(uint8_t light, uint8_t light1, uint8_t light2, uint8_t light3,
															    uint8_t light4,			        uint8_t light5,
														        uint8_t light6, uint8_t light7, uint8_t light8);
		std::array<glm::ivec3, 8> getNeighbourVoxels(const glm::ivec3& npos, uint8_t normalIndex);


		size_t size;
		size_t quadCount;

		MeshDelegates meshDelegates;
		gfx::VertexArray vao;

		gfx::DynamicBuffer<VertexData> vbo;
		gfx::DynamicBuffer<DrawIndirectCommand> icbo;
		std::vector<uint32_t> vertices;
		std::vector<uint32_t> indices;
	};
}