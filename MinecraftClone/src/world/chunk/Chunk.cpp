#include "pch.h"
#include "renderer/Renderer.h"
#include "world/chunk/Chunk.h"

Chunk::Chunk(const glm::ivec2& chunkPos, const std::vector<Block>& blockTypes)
	:blockTypes(blockTypes), chunkPos(chunkPos)
{
	blocks = new BlockID** [CHUNK_HEIGHT];
	for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
		blocks[j] = new BlockID* [CHUNK_DIM];
		for (size_t i = 0; i < CHUNK_DIM; i++) {
			blocks[j][i] = new BlockID[CHUNK_DIM];
			memset(blocks[j][i], 0, CHUNK_DIM * sizeof(BlockID));
		}
	}

	lightMap = new uint8_t** [CHUNK_HEIGHT];
	for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
		lightMap[j] = new uint8_t* [CHUNK_DIM];
		for (size_t i = 0; i < CHUNK_DIM; i++) {
			lightMap[j][i] = new uint8_t[CHUNK_DIM];
			memset(lightMap[j][i], 0, CHUNK_DIM * sizeof(uint8_t));
		}
	}

	vbo.allocate(CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 24, nullptr);

	vao.bindLayout(VertexArrayLayout{
		{
		   { 1, false },
		   { 1, false }
		}
		});
	vao.bindVertexBuffer(vbo.getTargetBuffer(), 0, VertexBufferLayout{
		{{ 0, 0 }, { 1, offsetof(VertexData, lightData)}},
		0,
		sizeof(VertexData)
		});
	vao.bindIndexBuffer(Renderer::GetChunkIbo());

	icbo.allocate(sizeof(DrawIndirectCommand), nullptr);
}

Chunk::~Chunk() {
	for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
		for (size_t i = 0; i < CHUNK_DIM; i++)
			delete[] blocks[j][i];
		delete[] blocks[j];
	}
	delete[] blocks;

	for (size_t j = 0; j < CHUNK_HEIGHT; j++) {
		for (size_t i = 0; i < CHUNK_DIM; i++)
			delete[] lightMap[j][i];
		delete[] lightMap[j];
	}
	delete[] lightMap;

	if (!neighbourChunks.east.expired())
		neighbourChunks.east.lock()->neighbourChunks.west.reset();
	if (!neighbourChunks.west.expired())
		neighbourChunks.west.lock()->neighbourChunks.east.reset();
	if (!neighbourChunks.south.expired())
		neighbourChunks.south.lock()->neighbourChunks.north.reset();
	if (!neighbourChunks.north.expired())
		neighbourChunks.north.lock()->neighbourChunks.south.reset();
}


void Chunk::pushQuad(const Quad& quad, const glm::uvec3& localPos, uint8_t textureID, uint8_t light) {
	for (const Vertex& vertex : quad.vertices) {
		uint32_t v = ((vertex.pos.y + localPos.y) << 22) | ((vertex.pos.x + localPos.x) << 17) | ((vertex.pos.z + localPos.z) << 12) | (vertex.texUV << 10) | (textureID << 2) | (vertex.shading);
		vbo.push({ v, light });
	}
}

void Chunk::buildCubeLayer() {
	vbo.beginEditRegion(0, CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 24);
	uint32_t quad_count = 0;
	for (uint8_t y = 0; y < CHUNK_HEIGHT; y++)
		for (uint8_t x = 0; x < CHUNK_DIM; x++)
			for (uint8_t z = 0; z < CHUNK_DIM; z++) {
				glm::ivec3 localPos = glm::ivec3(x, y, z);

				BlockID block = getBlock(localPos);

				if (!block)
					continue;

				Block type = blockTypes[block];
				if (true) {
					for (uint8_t i = 0; i < 6; i++) {
						if (canRenderFacing(localPos + IDIRECTIONS[i])) {
							uint8_t light = 15 << 4;
							Chunk* neighbourChunk = getNeighbourChunk(localPos + IDIRECTIONS[i]);
							if (neighbourChunk)
								light = neighbourChunk->getRawLight(ToLocalPos(localPos + IDIRECTIONS[i]));
							pushQuad(type.getModel().getQuads()[i], localPos, type.getTextureLayout()[i], light);
							quad_count++;
						}
					}
				}
				
			}
	vbo.endEditRegion();

	DrawIndirectCommand cmd;
	cmd.count = quad_count * 6;
	cmd.instanceCount = 1;
	cmd.firstIndex = 0;
	cmd.baseVertex = 0;
	cmd.baseInstance = 0;
	icbo.beginEditRegion(0, 1);
	icbo.editRegion(0, 1, &cmd);
	icbo.endEditRegion();
	drawable = true;
}

Chunk* Chunk::getNeighbourChunk(const glm::ivec3& localPos) {
	if (!IsOutside(localPos))
		return this;
	if (localPos.x >= CHUNK_DIM && !neighbourChunks.east.expired())
		return neighbourChunks.east.lock().get();
	if (localPos.z < 0 && !neighbourChunks.north.expired())
		return neighbourChunks.north.lock().get();
	if (localPos.x < 0 && !neighbourChunks.west.expired())
		return neighbourChunks.west.lock().get();
	if (localPos.z >= CHUNK_DIM && !neighbourChunks.south.expired())
		return neighbourChunks.south.lock().get();
	return nullptr;
}



void Chunk::updateLayers() {
	buildCubeLayer();
}
void Chunk::drawCubeLayer() {
	if (!drawable) 
		return;
	Renderer::MultiDrawElementsIndirect(vao, icbo.getTargetBuffer());
}