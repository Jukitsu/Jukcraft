#include "pch.h"
#include "renderer/Renderer.h"
#include "world/chunk/Chunk.h"
#include "renderer/models/Model.h"

Chunk::Chunk(const glm::ivec2& chunkPos) {
	blocks = new BlockID** [CHUNK_HEIGHT];
	for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
		blocks[j] = new BlockID* [CHUNK_DIM];
		for (size_t i = 0; i < CHUNK_DIM; i++) {
			blocks[j][i] = new BlockID[CHUNK_DIM];
			memset(blocks[j][i], 0, CHUNK_DIM * sizeof(BlockID));
		}
	}

	vao.emplace();
	vbo.emplace();

	vbo->allocate(CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 24, nullptr);

	vao->bindLayout(VertexArrayLayout{
		{
		   { 1, false },
		}
		});
	vao->bindVertexBuffer(vbo->getTargetBuffer(), 0, VertexBufferLayout{
		{{ 0 }},
		0,
		sizeof(uint32_t)
		});
	vao->bindIndexBuffer(Renderer::GetChunkIbo());


	icbo.emplace();
	icbo->allocate(sizeof(DrawIndirectCommand), nullptr);
}

Chunk::~Chunk() {
	for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
		for (size_t i = 0; i < CHUNK_DIM; i++)
			delete[] blocks[j][i];
		delete[] blocks[j];
	}
	delete[] blocks;
}


void Chunk::pushQuad(const Quad& quad, const glm::uvec3& localPos) {
	for (const Vertex& vertex : quad.vertices) {
		uint32_t v = ((vertex.pos.y + localPos.y) << 22) | ((vertex.pos.x + localPos.x) << 17) | ((vertex.pos.z + localPos.z) << 12) | (vertex.texUV << 10) | (vertex.texIndex << 2) | (vertex.shading);
		vbo->push(v);
	}
}

void Chunk::buildCubeLayer() {
	vbo->beginEditRegion(0, CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 24);
	uint32_t quad_count = 0;
	for (uint8_t y = 0; y < CHUNK_HEIGHT; y++)
		for (uint8_t x = 0; x < CHUNK_DIM; x++)
			for (uint8_t z = 0; z < CHUNK_DIM; z++) {
				glm::ivec3 localPos = glm::ivec3(x, y, z);
				uint16_t block = getBlock(localPos);
				if (!block)
					continue;
				if (canRenderFacing(localPos + glm::ivec3(EAST))) {
					pushQuad(cube[0], localPos);
					quad_count++;
				}
				if (canRenderFacing(localPos + glm::ivec3(WEST))) {
					pushQuad(cube[1], localPos);
					quad_count++;
				}
				if (canRenderFacing(localPos + glm::ivec3(UP))) {
					pushQuad(cube[2], localPos);
					quad_count++;
				}
				if (canRenderFacing(localPos + glm::ivec3(DOWN))) {
					pushQuad(cube[3], localPos);
					quad_count++;
				}
				if (canRenderFacing(localPos + glm::ivec3(SOUTH))) {
					pushQuad(cube[4], localPos);
					quad_count++;
				}
				if (canRenderFacing(localPos + glm::ivec3(NORTH))) {
					pushQuad(cube[5], localPos);
					quad_count++;
				}
				
			}
	vbo->endEditRegion();
	DrawIndirectCommand cmd;
	cmd.count = quad_count * 6;
	cmd.instanceCount = 1;
	cmd.firstIndex = 0;
	cmd.baseVertex = 0;
	cmd.baseInstance = 0;
	icbo->beginEditRegion(0, 1);
	icbo->editRegion(0, 1, &cmd);
	icbo->endEditRegion();
	drawable = true;
}
void Chunk::drawCubeLayer() {
	if (!drawable) 
		return;
	Renderer::MultiDrawElementsIndirect(*vao, icbo->getTargetBuffer());
}