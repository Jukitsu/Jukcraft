#pragma once

constexpr uint8_t CHUNK_DIM = 16;
constexpr uint8_t CHUNK_HEIGHT = 128;

constexpr uint8_t WORLD_SIZE = 12;


struct PerChunkData {
	glm::vec3 chunkPos;
};