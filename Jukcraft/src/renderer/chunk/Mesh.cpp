#include "pch.h"
#include "renderer/chunk/Mesh.h"

namespace Jukcraft {
	static constexpr float smooth(float a, float b, float c, float d) {
		if (!(a && b && c && d)) {
			std::array<float, 4> l = {
				a,
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity()
			};
			if (b)
				l[1] = b;
			if (c)
				l[2] = c;
			if (d)
				l[3] = d;
			float min_val = *std::min_element(l.begin(), l.end());
			float a = std::max(a, min_val);
			float b = std::max(b, min_val);
			float c = std::max(c, min_val);
			float d = std::max(d, min_val);
			return a + b + c + d / 4.0f;
		}

	}

	float ao(float s1, float s2, float c) {
		if (s1 && s2)
			return 0.25;

		return 1.0f - (s1 + s2 + c) / 4.0f;
	}

	Mesh::Mesh(size_t size) :size(size) {
		vbo.allocate(CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 24, nullptr);

		vao.bindLayout(gfx::VertexArrayLayout{
			{
			   { 1, false },
			   { 1, false }
			}
			});
		vao.bindVertexBuffer(vbo.getTargetBuffer(), 0, gfx::VertexBufferLayout{
			{{ 0, 0 }, { 1, offsetof(VertexData, lightData)}},
			0,
			sizeof(VertexData)
			});
		vao.bindIndexBuffer(Renderer::GetChunkIbo());

		icbo.allocate(sizeof(DrawIndirectCommand), nullptr);
	}

	Mesh::~Mesh() {

	}

	void Mesh::begin() {
		vbo.beginEditRegion(0, size);
		quadCount = 0;
	}

	void Mesh::pushQuad(const Quad& quad, const glm::uvec3& localPos, uint8_t textureID, uint8_t light) {
		for (const Vertex& vertex : quad.vertices) {
			uint32_t v = ((vertex.pos.y + localPos.y) << 22) | ((vertex.pos.x + localPos.x) << 17) | ((vertex.pos.z + localPos.z) << 12) | (vertex.texUV << 10) | (textureID << 2) | (vertex.shading);
			vbo.push({ v, light });
		}
		quadCount++;
	}
	
	void Mesh::end() {
		vbo.endEditRegion();
		DrawIndirectCommand cmd;
		cmd.count = quadCount * 6;
		cmd.instanceCount = 1;
		cmd.firstIndex = 0;
		cmd.baseVertex = 0;
		cmd.baseInstance = 0;
		icbo.beginEditRegion(0, 1);
		icbo.editRegion(0, 1, &cmd);
		icbo.endEditRegion();
	}

	void Mesh::draw() {
		Renderer::MultiDrawElementsIndirect(vao, icbo.getTargetBuffer());
	}


}