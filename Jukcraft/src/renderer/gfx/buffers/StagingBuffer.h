#pragma once
#include "renderer/gfx/objects/Buffer.h"

namespace Jukcraft {
	namespace gfx {

		constexpr size_t STAGING_BUFFER_CAPACITY = 1 << 24;

		template<typename T>
		struct Arena {
			size_t offset;
			T* pointer;
			void* source;
			size_t size;
		};

		class StagingBuffer {
		public:
			StagingBuffer() {
				buffer.allocate(STAGING_BUFFER_CAPACITY, nullptr, true);
				mappedStagingBuffer = reinterpret_cast<uint8_t*>(buffer.map(0, STAGING_BUFFER_CAPACITY));
				freeList.push_back({ 0, STAGING_BUFFER_CAPACITY });
			}

			template<typename T>
			const Arena<T> allocateArena(size_t length) {	

				Arena<T> arena;
				arena.source = mappedStagingBuffer;
				arena.size = length * sizeof(T);

				std::unique_lock<std::mutex> lock(mutex);

				while (true) {
					for (auto it = freeList.begin(); it != freeList.end(); ++it) {
						if (it->size >= arena.size) {
							// Found a free block that can accommodate the allocation
							arena.offset = it->start;
							arena.pointer = reinterpret_cast<T*>(mappedStagingBuffer + it->start);
							it->start += arena.size;
							it->size -= arena.size;

							if (it->size == 0) {
								// Remove the block from the free list if it's fully allocated
								freeList.erase(it);
							}

							return arena;
						}
					}

					// If no suitable block found, wait for free space
					condition.wait(lock);
				}
			}

			template<typename T>
			void freeArena(const Arena<T>& arena) {

				std::lock_guard<std::mutex> lock(mutex);
				// Add the freed block to the free list
				freeList.push_back({ arena.offset, arena.size });
				// Sort the free list by start address for faster allocation search
				freeList.sort([](const FreeBlock& a, const FreeBlock& b) {
					return a.start < b.start;
					});
				condition.notify_all();
			}

			constexpr void* getMappedStagingBuffer() const noexcept { return mappedStagingBuffer; }
			constexpr Buffer& getBuffer() noexcept { return buffer;  }

		private:
			Buffer buffer;
			uint8_t* mappedStagingBuffer;
			
			struct FreeBlock {
				size_t start;
				size_t size;
			};

			std::list<FreeBlock> freeList;
			std::mutex mutex;
			std::condition_variable condition;

			FORBID_COPY(StagingBuffer);
			FORBID_MOVE(StagingBuffer);
		};
	}
}