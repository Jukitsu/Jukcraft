#pragma once

namespace Jukcraft {
	constexpr uint8_t CHUNK_DIM = 16;
	constexpr uint8_t CHUNK_HEIGHT = 128;

	constexpr uint8_t WORLD_SIZE = 2;


	struct PerChunkData {
		glm::vec3 chunkPos;
	};
}