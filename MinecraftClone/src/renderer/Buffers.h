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

	[[nodiscard]] constexpr GLuint getHandle() const { return handle; }
private:
	GLuint handle;
	FORBID_COPY(Buffer);
	FORBID_MOVE(Buffer);
};

struct VertexBufferLayout {
	struct BufferElement {
		size_t attribIndex;
		size_t relativeOffset;
	};
	std::vector<BufferElement> elements;
	size_t offset;
	size_t stride;
};