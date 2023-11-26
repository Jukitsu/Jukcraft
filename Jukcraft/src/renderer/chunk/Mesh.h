#pragma once
#include "renderer/gfx/buffers/DynamicBuffer.h"
#include "blocks/Block.h"

namespace Jukcraft {

	class Mesh {
	public:
		Mesh(size_t size);
		~Mesh();
		void begin();
		void pushQuad(const Quad& quad, const glm::uvec3& localPos, uint8_t textureID, uint8_t light);
		void end();
		void draw();
	private:
		size_t size;
		size_t quadCount;
		gfx::VertexArray vao;

		gfx::DynamicBuffer<VertexData> vbo;
		gfx::DynamicBuffer<DrawIndirectCommand> icbo;
		std::vector<uint32_t> vertices;
		std::vector<uint32_t> indices;
	};
}