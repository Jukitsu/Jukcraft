#pragma once
#include "renderer/gfx/objects/Buffer.h"
#include "renderer/Renderer.h"
#include "core/Window.h"

namespace Jukcraft {


	namespace gfx {
		Window& GetMainWindow() noexcept;

		template<typename T>
		class DynamicBuffer {
		public:
			DynamicBuffer();

			void allocate(uint32_t length, const T* data);
			void beginEditRegion(uint32_t offset, uint32_t length);
			void editRegion(uint32_t offset, uint32_t length, const T* data);
			void push(const T& data);
			void endEditRegion();

			constexpr Buffer& getTargetBuffer() noexcept { return targetBuffer; }
		private:
			struct {
				size_t offset, length;
			} regionData;
			Buffer targetBuffer;
			Arena<T> arena;
			size_t currentOffset;
			std::optional<SharedContext> sharedContext;

			FORBID_COPY(DynamicBuffer);
			FORBID_MOVE(DynamicBuffer);
		};

		template<typename T>
		inline DynamicBuffer<T>::DynamicBuffer()
			:targetBuffer() {

		}

		template<typename T>
		inline void DynamicBuffer<T>::allocate(uint32_t length, const T* data) {
			targetBuffer.allocate(length * sizeof(T), data, false);
		}

		template<typename T>
		inline void DynamicBuffer<T>::beginEditRegion(uint32_t offset, uint32_t length) {
			// sharedContext.emplace(GetMainWindow());
			arena = Renderer::AllocateArena<T>(length);
			// sharedContext->makeCurrent();
			Renderer::GetStagingBuffer().sync();
			if (length > STAGING_BUFFER_CAPACITY)
				THROW_ERROR("Staging Buffer Overflow")
			currentOffset = 0;
			regionData = { offset, length };
		}

		template<typename T>
		inline void DynamicBuffer<T>::editRegion(uint32_t offset, uint32_t length, const T* data) {
			memcpy(arena.pointer + regionData.offset + offset, data, length * sizeof(T));
		}

		template<typename T>
		inline void DynamicBuffer<T>::push(const T& data) {
			memcpy(arena.pointer + regionData.offset + currentOffset, &data, sizeof(T));
			currentOffset++;
		}

		template<typename T>
		inline void DynamicBuffer<T>::endEditRegion() {
			Renderer::GetStagingBuffer().flush(arena.offset + regionData.offset * sizeof(T), regionData.length * sizeof(T));
			targetBuffer.copy(Renderer::GetStagingBuffer(), arena.offset + regionData.offset * sizeof(T), regionData.offset * sizeof(T), regionData.length * sizeof(T));
			Renderer::FreeArena(arena);
			// sharedContext.reset();
			currentOffset = 0;
		}


	}
}