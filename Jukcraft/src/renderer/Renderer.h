#pragma once
#include "renderer/gfx/objects/VertexArray.h"
#include "renderer/gfx/objects/Buffer.h"
#include "renderer/gfx/objects/Shader.h"
#include "renderer/texture/TextureManager.h"



namespace Jukcraft {

	inline static const glm::vec4 empty_color = glm::vec4(0.0f);
	inline static constexpr float empty_depth = 1.0f;


	class Renderer {
	public:
		static void Init() {
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_FRAMEBUFFER_SRGB);
			glEnable(GL_MULTISAMPLE);

			std::vector<uint32_t> indices;
			indices.resize(CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 36);
			for (uint32_t nquad = 0; nquad < CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 6; nquad++) {

				indices[(size_t)nquad * 6] = 4 * nquad + 0;
				indices[(size_t)nquad * 6 + 1] = 4 * nquad + 1;
				indices[(size_t)nquad * 6 + 2] = 4 * nquad + 2;
				indices[(size_t)nquad * 6 + 3] = 4 * nquad + 0;
				indices[(size_t)nquad * 6 + 4] = 4 * nquad + 2;
				indices[(size_t)nquad * 6 + 5] = 4 * nquad + 3;
			}
			chunkIbo.emplace();
			chunkIbo->allocate(sizeof(uint32_t) * indices.size(), indices.data(), false);

			stagingBuffer.emplace();
			stagingBuffer->allocate(16777216, nullptr, true);
			mappedStagingBuffer = stagingBuffer->map(0, 16777216);


		}
		static void Begin(const glm::vec4& skyColor) {
			while (fences.size() > 3) {
				GLsync fence = fences.front();
				glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 2147483647);
				glDeleteSync(fence);
				fences.pop();
			}

			glClearNamedFramebufferfv(0, GL_COLOR, 0, glm::value_ptr(skyColor));
			glClearNamedFramebufferfv(0, GL_DEPTH, 0, &empty_depth);
		}
		static void DrawArrays(gfx::VertexArray& vao, uint32_t first, uint32_t count) {
			vao.bind();
			glDrawArrays(GL_TRIANGLES, first, count);
		}
		static void DrawElements(gfx::VertexArray& vao, uint32_t count) {
			vao.bind();
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		}
		static void MultiDrawElementsIndirect(gfx::VertexArray& vao, gfx::Buffer& icbo) {
			vao.bind();
			icbo.bind(gfx::BufferBindingTarget::DrawIndirect);
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 1, 0);
		}
		static void End() {
			fences.push(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
		}
		static void Finish() {
			while (!fences.empty()) {
				glClientWaitSync(fences.front(), GL_SYNC_FLUSH_COMMANDS_BIT, 2147483647);
				glDeleteSync(fences.front());
				fences.pop();
			}
		}
		static void Viewport(uint16_t width, uint16_t height) {
			glViewport(0, 0, width, height);
		}
		static constexpr const gfx::Buffer& GetChunkIbo() {
			return *chunkIbo;
		}
		static constexpr gfx::Buffer& GetStagingBuffer() {
			return *stagingBuffer;
		}
		static void* GetMappedStagingBuffer() {
			return mappedStagingBuffer;
		}
	private:
		inline static std::queue<GLsync> fences;
		inline static std::queue<std::pair<const gfx::Buffer*, GLsync>> bufferWriteFences;
		inline static std::optional<gfx::Buffer> chunkIbo;
		inline static std::optional<gfx::Buffer> stagingBuffer;
		inline static void* mappedStagingBuffer;
	};
}