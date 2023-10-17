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
			mapped ? GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_CLIENT_STORAGE_BIT : 0
		);
	}
	[[nodiscard]] void* map(size_t offset, size_t length) {
		return glMapNamedBufferRange(
			handle,
			offset,
			length,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_INVALIDATE_RANGE_BIT
		);
	}
	
	void flush(size_t offset, size_t length) {
		/*
		glFlushMappedNamedBufferRange(
			handle,
			offset,
			length
		);
		*/
	}
	void bindRange(BufferBindingTarget target, uint32_t index, size_t offset, size_t size) {
		glBindBufferRange(getGLBufferBindingTarget(target), index, handle, offset, size);
	}
	void bind(BufferBindingTarget target) {
		glBindBuffer(getGLBufferBindingTarget(target), handle);
	}
	void copy(const Buffer& buffer, size_t readOffset, size_t writeOffset, size_t size) {
		buffer.sync();
		glCopyNamedBufferSubData(buffer.handle, handle, readOffset, writeOffset, size);
		addFence();
		buffer.addFence();
	}
	void orphan(size_t offset, size_t length) {
		glInvalidateBufferSubData(handle, offset, length);
		addFence();
	}
	[[nodiscard]] constexpr GLuint getHandle() const { return handle; }

	void addFence() const {
		fences.push(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
	}
	void sync() const {
		while (!fences.empty()) {
			glClientWaitSync(fences.front(), GL_SYNC_FLUSH_COMMANDS_BIT, 2147483647);
			glDeleteSync(fences.front());
			fences.pop();
		}
	}
private:
	GLuint handle;
	mutable std::queue<GLsync> fences;
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

