#pragma once
#include "renderer/gfx/objects/Buffer.h"
#include "renderer/Renderer.h"

template<typename T>
class DynamicBuffer {
public:
	DynamicBuffer() :targetBuffer() {}

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
	T* mappedPtr;
	size_t currentOffset;

	FORBID_COPY(DynamicBuffer);
	FORBID_MOVE(DynamicBuffer);
};

#include "renderer/gfx/buffers/DynamicBuffer.inl"