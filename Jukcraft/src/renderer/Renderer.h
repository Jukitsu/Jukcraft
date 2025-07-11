#pragma once
#include "renderer/gfx/objects/VertexArray.h"
#include "renderer/gfx/objects/Buffer.h"
#include "renderer/gfx/buffers/StagingBuffer.h"
#include "renderer/gfx/objects/Shader.h"
#include "renderer/gfx/objects/Fence.h"
#include "renderer/texture/TextureManager.h"



namespace Jukcraft {

	inline static const glm::vec4 empty_color = glm::vec4(0.0f);
	inline static constexpr float empty_depth = 1.0f;


	class Renderer {
	public:
		static void Init() {
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_MULTISAMPLE);
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			// glEnable(GL_FRAMEBUFFER_SRGB);

			/* Initialize the general quad index buffer */
			std::vector<uint32_t> indices;
			indices.resize(MAX_QUADS * 6);
			for (uint32_t nquad = 0; nquad < MAX_QUADS; nquad++) {

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
			mappedStagingBuffer = stagingBuffer->getMappedStagingBuffer();


		}
		static void Begin(const glm::vec4& skyColor) {
			/* Wait for GPU tasks if they are 3 frames in */
			while (fences.size() > 3) {
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
			fences.emplace();
		}
		static void Finish() {
			while (!fences.empty()) {
				fences.pop();
			}
		}
		static void Viewport(uint16_t width, uint16_t height) {
			glViewport(0, 0, width, height);
		}
		static constexpr const gfx::Buffer& GetChunkIbo() noexcept {
			return *chunkIbo;
		}
		template<typename T>
		static const gfx::Arena<T> AllocateArena(size_t size) noexcept {
			return stagingBuffer->allocateArena<T>(size);
		}
		template<typename T>
		static void FreeArena(const gfx::Arena<T>& arena) noexcept {
			stagingBuffer->freeArena<T>(arena);
		}
		static constexpr gfx::Buffer& GetStagingBuffer() noexcept {
			return stagingBuffer->getBuffer();
		}
		static void* GetMappedStagingBuffer() noexcept {
			return mappedStagingBuffer;
		}
	private:
		inline static std::queue<Fence> fences;
		inline static std::queue<std::pair<const gfx::Buffer*, GLsync>> bufferWriteFences;
		inline static Nullable<gfx::Buffer> chunkIbo;
		inline static Nullable<gfx::StagingBuffer> stagingBuffer;
		inline static void* mappedStagingBuffer;
	};
}