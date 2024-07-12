#pragma once
#include "renderer/gfx/objects/Buffer.h"
#include "renderer/Renderer.h"

namespace Jukcraft {

	
	namespace gfx {

		template<typename T>
		class StreamingBuffer {
		public:
			StreamingBuffer();

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
			Buffer buffer;
			T* mappedPointer;
			size_t currentOffset;

			FORBID_COPY(StreamingBuffer);
			FORBID_MOVE(StreamingBuffer);
		};

		template<typename T>
		inline StreamingBuffer<T>::StreamingBuffer()
			:buffer() {

		}

		template<typename T>
		inline void StreamingBuffer<T>::allocate(uint32_t length, const T* data) {
			buffer.allocate(length * sizeof(T), data, true);
			mappedPointer = buffer.map(0, length * sizeof(T));
		}

		template<typename T>
		inline void StreamingBuffer<T>::beginEditRegion(uint32_t offset, uint32_t length) {
			buffer.sync();
			if (length > STAGING_BUFFER_CAPACITY)
				THROW_ERROR("Staging Buffer Overflow");
			currentOffset = 0;
			regionData = { offset, length };
		}

		template<typename T>
		inline void StreamingBuffer<T>::editRegion(uint32_t offset, uint32_t length, const T* data) {
			memcpy(arena.pointer + regionData.offset + offset, data, length * sizeof(T));
		}

		template<typename T>
		inline void StreamingBuffer<T>::push(const T& data) {
			memcpy(arena.pointer + regionData.offset + currentOffset, &data, sizeof(T));
			currentOffset++;
		}

		template<typename T>
		inline void StreamingBuffer<T>::endEditRegion() {
			Renderer::GetStagingBuffer().flush(arena.offset + regionData.offset * sizeof(T), regionData.length * sizeof(T));
			targetBuffer.copy(Renderer::GetStagingBuffer(), arena.offset + regionData.offset * sizeof(T), regionData.offset * sizeof(T), regionData.length * sizeof(T));
			Renderer::FreeArena(arena);
			// sharedContext.unbind();
			currentOffset = 0;
		}


	}
}
