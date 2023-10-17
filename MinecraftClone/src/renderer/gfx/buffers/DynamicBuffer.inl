template<typename T>
inline void DynamicBuffer<T>::allocate(uint32_t length, const T* data) {
	mappedPtr = reinterpret_cast<T*>(Renderer::GetMappedStagingBuffer());
	targetBuffer.allocate(length * sizeof(T), data, false);
}

template<typename T>
inline void DynamicBuffer<T>::beginEditRegion(uint32_t offset, uint32_t length) {
	Renderer::GetStagingBuffer().sync();
	if (length > 16777216)
		THROW_ERROR("Staging Buffer Overflow")
		currentOffset = 0;
	regionData = { offset, length };
}

template<typename T>
inline void DynamicBuffer<T>::editRegion(uint32_t offset, uint32_t length, const T* data) {
	memcpy(mappedPtr + regionData.offset + offset, data, length * sizeof(T));
}

template<typename T>
inline void DynamicBuffer<T>::push(const T& data) {
	memcpy(mappedPtr + regionData.offset + currentOffset, &data, sizeof(T));
	currentOffset++;
}

template<typename T>
inline void DynamicBuffer<T>::endEditRegion() {
	targetBuffer.copy(Renderer::GetStagingBuffer(), regionData.offset * sizeof(T), regionData.offset * sizeof(T), regionData.length * sizeof(T));
	currentOffset = 0;
}