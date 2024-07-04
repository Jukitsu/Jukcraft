#include "pch.h"
#include "renderer/chunk/RenderRegion.h"

namespace Jukcraft {
	RenderRegion::RenderRegion(std::array<Shared<Chunk>, RENDER_REGION_SIZE>&& chunks) 
		:chunks(std::move(chunks)){
		vbo.allocate(MAX_QUADS * 4, nullptr);

		vao.bindLayout(
			gfx::VertexArrayLayout{
				{
				   { 1, false },
				   { 1, false }
				}
			}
		);
		vao.bindVertexBuffer(
			vbo.getTargetBuffer(), 0,
			gfx::VertexBufferLayout{
				{{ 0, 0 }, { 1, offsetof(VertexData, smoothLightData)}},
				0,
				sizeof(VertexData)
			}
		);
		vao.bindIndexBuffer(Renderer::GetChunkIbo());

		icbo.allocate(sizeof(DrawIndirectCommand), nullptr);
	}

	void RenderRegion::draw() {

	}
}
