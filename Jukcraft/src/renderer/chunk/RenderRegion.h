#pragma once
#include "renderer/gfx/buffers/DynamicBuffer.h"
#include "renderer/gfx/buffers/StreamingBuffer.h"
#include "blocks/Block.h"

namespace Jukcraft {
	class Chunk;

	class RenderRegion {
	public:
		RenderRegion(std::array<Shared<Chunk>, RENDER_REGION_SIZE>&& chunks);
		void draw();
	private:
		std::array<Shared<Chunk>, RENDER_REGION_SIZE> chunks;
		gfx::VertexArray vao;

		gfx::DynamicBuffer<VertexData> vbo;
		gfx::DynamicBuffer<DrawIndirectCommand> icbo;
	};
}
