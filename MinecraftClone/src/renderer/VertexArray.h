#pragma once
#include <glad/glad.h>
#include "renderer/Buffers.h"

struct VertexArrayLayout {
	struct VertexAttrib {
		size_t size;
		bool is_float;
	};
	std::vector<VertexAttrib> attribs;
};

class VertexArray {
public:
	VertexArray() {
		glCreateVertexArrays(1, &handle);
	}
	~VertexArray() {
		glDeleteVertexArrays(1, &handle);
	}
	void bind() {
		glBindVertexArray(handle);
	}
	void bindLayout(VertexArrayLayout&& layout) {
		vaLayout = std::move(layout);
	}
	void bindVertexBuffer(Buffer& vbo, size_t offset, VertexBufferLayout layout) {
		uint32_t binding = buffers.size();
		glVertexArrayVertexBuffer(handle, binding, vbo.getHandle(), layout.offset, layout.stride);
		buffers.push_back(&vbo);
		for (const auto& element : layout.elements) {
			const auto& attrib = vaLayout.attribs[element.attribIndex];
			glEnableVertexArrayAttrib(handle, element.attribIndex);
			glVertexArrayAttribBinding(handle, element.attribIndex, binding);
			if (attrib.is_float)
				glVertexArrayAttribFormat(handle, element.attribIndex, attrib.size, GL_FLOAT, GL_FALSE, element.relativeOffset);
			else
				glVertexArrayAttribIFormat(handle, element.attribIndex, attrib.size, GL_UNSIGNED_INT, element.relativeOffset);
		}
	}
	void bindIndexBuffer(Buffer& ibo) {
		glVertexArrayElementBuffer(handle, ibo.getHandle());
	}
	[[nodiscard]] constexpr GLuint getHandle() const { return handle; }
private:
	GLuint handle;
	VertexArrayLayout vaLayout;
	std::vector<Buffer*> buffers;

	FORBID_COPY(VertexArray);
	FORBID_MOVE(VertexArray);
};

using Batch = VertexArray;
