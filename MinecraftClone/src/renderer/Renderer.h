#pragma once
#include "renderer/gfx/VertexArray.h"
#include "renderer/gfx/Buffers.h"
#include "renderer/gfx/Shader.h"
#include "renderer/texture/TextureManager.h"

inline static constexpr glm::vec4 empty_color = glm::vec4(0.0f);
inline static constexpr float empty_depth = 1.0f;

class Renderer {
public:
	static void Init() {
		glEnable(GL_DEPTH_TEST);
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
	static void DrawArrays(VertexArray& vao, size_t first, size_t count) {
		vao.bind();
		glDrawArrays(GL_TRIANGLES, first, count);
	}
	static void DrawElements(VertexArray& vao, size_t count) {
		vao.bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
	static void End() {
		fences.push(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
	}
	static void Viewport(uint16_t width, uint16_t height) {
		glViewport(0, 0, width, height);
	}
private:
	inline static std::queue<GLsync> fences;
};