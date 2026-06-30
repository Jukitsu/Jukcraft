#pragma once
#include <array>
#include <memory>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <glad/glad.h>
#include "renderer/gfx/objects/Buffer.h"
#include "renderer/gfx/objects/Fence.h"

namespace Jukcraft {
    namespace gfx {

        template<typename T>
        class StreamingBuffer {
        public:
            StreamingBuffer() = default;
            ~StreamingBuffer() = default;

            // Public methods
            void allocate(uint32_t length, const T* data = nullptr);
            void beginEditRegion(uint32_t offset, uint32_t length);
            void editRegion(uint32_t offset, uint32_t length, const T* data);
            void push(const T& data);
            void endEditRegion();

            Buffer& getTargetBuffer() noexcept;
            void bind(BufferBindingTarget target) const;
            void bindRange(BufferBindingTarget target, uint32_t index,
                size_t offsetBytes, size_t sizeBytes) const;

        private:
            // Private helper
            T* mapNextChunk(size_t& outByteOffset);

            Buffer m_buffer;
            size_t m_elementsPerChunk = 0;
            size_t m_chunkSizeBytes = 0;
            size_t m_totalSizeBytes = 0;
            void* m_mappedPtr = nullptr;

            int m_currentSlot = 0;
            size_t m_currentOffsetBytes = 0;

            // Region state
            size_t m_regionOffsetBytes = 0;
            size_t m_regionLengthBytes = 0;
            size_t m_writeCursor = 0;

            // Three fences – one per chunk
            std::array<std::unique_ptr<Fence>, 3> m_fences = { nullptr, nullptr, nullptr };

            FORBID_COPY(StreamingBuffer);
            FORBID_MOVE(StreamingBuffer);
        };

        // ---------------------------------------------------------------------
        // Public method definitions
        // ---------------------------------------------------------------------

        template<typename T>
        inline void StreamingBuffer<T>::allocate(uint32_t length, const T* data) {
            m_elementsPerChunk = length;
            m_chunkSizeBytes = length * sizeof(T);
            m_totalSizeBytes = m_chunkSizeBytes * 3;

            m_buffer.allocate(m_totalSizeBytes, nullptr, true);

            m_mappedPtr = m_buffer.map(0, m_totalSizeBytes);
            if (m_mappedPtr == nullptr) {
                throw std::runtime_error("StreamingBuffer: Failed to persistently map buffer");
            }

            if (data) {
                std::memcpy(m_mappedPtr, data, m_chunkSizeBytes);
                glFlushMappedNamedBufferRange(m_buffer.getHandle(), 0, m_chunkSizeBytes);
                m_fences[0] = std::make_unique<Fence>();
            }

            m_currentSlot = 0;
            m_currentOffsetBytes = 0;
            m_regionOffsetBytes = 0;
            m_regionLengthBytes = 0;
            m_writeCursor = 0;
        }

        template<typename T>
        inline void StreamingBuffer<T>::beginEditRegion(uint32_t offset, uint32_t length) {
            size_t chunkByteOffset = 0;
            T* chunkPtr = mapNextChunk(chunkByteOffset);

            m_regionOffsetBytes = offset * sizeof(T);
            m_regionLengthBytes = length * sizeof(T);
            m_writeCursor = 0;
        }

        template<typename T>
        inline void StreamingBuffer<T>::editRegion(uint32_t offset, uint32_t length, const T* data) {
            size_t byteOffset = m_regionOffsetBytes + offset * sizeof(T);
            size_t byteLength = length * sizeof(T);

            if (byteOffset + byteLength > m_regionOffsetBytes + m_regionLengthBytes) {
                byteLength = m_regionOffsetBytes + m_regionLengthBytes - byteOffset;
            }

            void* dest = static_cast<char*>(m_mappedPtr) + byteOffset;
            std::memcpy(dest, data, byteLength);
        }

        template<typename T>
        inline void StreamingBuffer<T>::push(const T& data) {
            size_t byteOffset = m_regionOffsetBytes + m_writeCursor * sizeof(T);
            if (byteOffset + sizeof(T) > m_regionOffsetBytes + m_regionLengthBytes) {
                return;
            }
            void* dest = static_cast<char*>(m_mappedPtr) + byteOffset;
            std::memcpy(dest, &data, sizeof(T));
            ++m_writeCursor;
        }

        template<typename T>
        inline void StreamingBuffer<T>::endEditRegion() {
            m_buffer.flush(m_regionOffsetBytes, m_regionLengthBytes);

            if (m_fences[m_currentSlot] != nullptr) {
                m_fences[m_currentSlot].reset();
            }
            m_fences[m_currentSlot] = std::make_unique<Fence>();

            m_regionOffsetBytes = 0;
            m_regionLengthBytes = 0;
            m_writeCursor = 0;
        }

        template<typename T>
        inline Buffer& StreamingBuffer<T>::getTargetBuffer() noexcept {
            return m_buffer;
        }

        template<typename T>
        inline void StreamingBuffer<T>::bind(BufferBindingTarget target) const {
            m_buffer.bind(target);
        }

        template<typename T>
        inline void StreamingBuffer<T>::bindRange(BufferBindingTarget target, uint32_t index,
            size_t offsetBytes, size_t sizeBytes) const {
            m_buffer.bindRange(target, index, offsetBytes, sizeBytes);
        }

        // ---------------------------------------------------------------------
        // Private method definitions
        // ---------------------------------------------------------------------

        template<typename T>
        inline T* StreamingBuffer<T>::mapNextChunk(size_t& outByteOffset) {
            // Try to find a free slot without blocking
            for (int i = 0; i < 3; ++i) {
                int slot = (m_currentSlot + i) % 3;

                if (m_fences[slot] == nullptr) {
                    m_currentSlot = slot;
                    m_currentOffsetBytes = slot * m_chunkSizeBytes;
                    return reinterpret_cast<T*>(static_cast<char*>(m_mappedPtr) + m_currentOffsetBytes);
                }

                // Non‑blocking poll using Fence::isSignaled()
                if (m_fences[slot]->isSignaled()) {
                    m_fences[slot].reset();
                    m_currentSlot = slot;
                    m_currentOffsetBytes = slot * m_chunkSizeBytes;
                    return reinterpret_cast<T*>(static_cast<char*>(m_mappedPtr) + m_currentOffsetBytes);
                }
            }

            // All 3 slots are busy – block on the current slot
            int slot = m_currentSlot;
            m_fences[slot].reset();

            m_currentSlot = slot;
            m_currentOffsetBytes = slot * m_chunkSizeBytes;
            return reinterpret_cast<T*>(static_cast<char*>(m_mappedPtr) + m_currentOffsetBytes);
        }

    } // namespace gfx
} // namespace Jukcraft