#pragma once
#include <glad/glad.h>

enum class BufferBindingTarget {
	ShaderStorage, Uniform, DrawIndirect
};

static constexpr GLenum getGLBufferBindingTarget(BufferBindingTarget target) {
	switch (target) {
	case BufferBindingTarget::ShaderStorage:
		return GL_SHADER_STORAGE_BUFFER;
	case BufferBindingTarget::Uniform:
		return GL_UNIFORM_BUFFER;
	case BufferBindingTarget::DrawIndirect:
		return GL_DRAW_INDIRECT_BUFFER;
	default:
		return 0;
	}
	return 0;
}
class Buffer {
public:
	Buffer() {
		glCreateBuffers(1, &handle);
	}
	~Buffer() {
		glDeleteBuffers(1, &handle);
	}
	void allocate(size_t size, const void* data, bool mapped) {
		glNamedBufferStorage(
			handle,
			size,
			data,
			mapped ? GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT : 0
		);
	}
	[[nodiscard]] void* map(size_t offset, size_t length) {
		return glMapNamedBufferRange(
			handle,
			offset,
			length,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
		);
	}
	void flush(size_t offset, size_t length) {
		glFlushMappedNamedBufferRange(
			handle,
			offset,
			length
		);
	}
	void bindRange(BufferBindingTarget target, uint32_t index, size_t offset, size_t size) {
		glBindBufferRange(getGLBufferBindingTarget(target), index, handle, offset, size);
	}
	void bind(BufferBindingTarget target) {
		glBindBuffer(getGLBufferBindingTarget(target), handle);
	}
	void copy(const Buffer& buffer, size_t readOffset, size_t writeOffset, size_t size) {
		glCopyNamedBufferSubData(buffer.handle, handle, readOffset, writeOffset, size);
	}
	[[nodiscard]] constexpr GLuint getHandle() const { return handle; }
private:
	GLuint handle;
	FORBID_COPY(Buffer);
	FORBID_MOVE(Buffer);
};

struct VertexBufferLayout {
	struct BufferElement {
		uint32_t attribIndex;
		uint32_t relativeOffset;
	};
	std::vector<BufferElement> elements;
	uint32_t offset;
	uint32_t stride;
};

template<typename T>
class DynamicBuffer {
public:
	DynamicBuffer() :stagingBuffer(), targetBuffer() {}

	void allocate(uint32_t length, const T* data) noexcept {
		stagingBuffer.allocate(length * sizeof(T), data, true);
		mappedPtr = reinterpret_cast<T*>(stagingBuffer.map(0, length * sizeof(T)));
		targetBuffer.allocate(length * sizeof(T), data, false);
	}
	void beginEditRegion(uint32_t offset, uint32_t length) noexcept {
		currentOffset = 0;
		regionData = { offset, length };
	}
	void editRegion(uint32_t offset, uint32_t length, const T* data) noexcept {
		memcpy(mappedPtr + offset, data, length * sizeof(T));
		stagingBuffer.flush(offset, length * sizeof(T));
	}
	void push(const T& data) {
		memcpy(mappedPtr + currentOffset, &data, sizeof(T));
		currentOffset++;
	}
	void endEditRegion() noexcept {
		stagingBuffer.flush(0, sizeof(T));
		targetBuffer.copy(stagingBuffer, regionData.offset * sizeof(T), regionData.offset * sizeof(T), regionData.length * sizeof(T));
		currentOffset = 0;
	}

	constexpr Buffer& getTargetBuffer() noexcept { return targetBuffer; }
private:
	struct {
		size_t offset, length;
	} regionData;
	Buffer stagingBuffer;
	Buffer targetBuffer;
	T* mappedPtr;
	size_t currentOffset;

	FORBID_COPY(DynamicBuffer);
	FORBID_MOVE(DynamicBuffer);
};