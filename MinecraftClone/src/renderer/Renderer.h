#pragma once
#include "renderer/gfx/VertexArray.h"
#include "renderer/gfx/Buffers.h"
#include "renderer/gfx/Shader.h"
#include "renderer/texture/TextureManager.h"

inline static const glm::vec4 empty_color = glm::vec4(0.0f);
inline static constexpr float empty_depth = 1.0f;

class Renderer {
public:
	static void Init() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		std::vector<uint32_t> indices;
		indices.resize(CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 36);
		for (uint32_t nquad = 0; nquad < CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 6; nquad++) {

			indices[nquad * 6] = 4 * nquad + 0;
			indices[nquad * 6 + 1] = 4 * nquad + 1;
			indices[nquad * 6 + 2] = 4 * nquad + 2;
			indices[nquad * 6 + 3] = 4 * nquad + 0;
			indices[nquad * 6 + 4] = 4 * nquad + 2;
			indices[nquad * 6 + 5] = 4 * nquad + 3;
		}
		chunkIbo.emplace();
		chunkIbo->allocate(sizeof(uint32_t) * indices.size(), indices.data(), false);
	}
	static void Begin() {
		while (fences.size() > 3) {
			GLsync fence = fences.front();
			glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 2147483647);
			glDeleteSync(fence);
			fences.pop();
		}

		glClearNamedFramebufferfv(0, GL_COLOR, 0, glm::value_ptr(empty_color));
		glClearNamedFramebufferfv(0, GL_DEPTH, 0, &empty_depth);
	}
	static void DrawArrays(VertexArray& vao, uint32_t first, uint32_t count) {
		vao.bind();
		glDrawArrays(GL_TRIANGLES, first, count);
	}
	static void DrawElements(VertexArray& vao, uint32_t count) {
		vao.bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
	static void MultiDrawElementsIndirect(VertexArray& vao, Buffer& icbo) {
		vao.bind();
		icbo.bind(BufferBindingTarget::DrawIndirect);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 1, 0);
	}
	static void End() {
		fences.push(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
	}
	static void Viewport(uint16_t width, uint16_t height) {
		glViewport(0, 0, width, height);
	}
	static constexpr const Buffer& GetChunkIbo() {
		return *chunkIbo;
	}
private:
	inline static std::queue<GLsync> fences;
	inline static std::optional<Buffer> chunkIbo;
};