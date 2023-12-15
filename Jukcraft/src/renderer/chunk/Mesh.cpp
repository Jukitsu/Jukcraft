#include "pch.h"
#include "world/chunk/Chunk.h"
#include "renderer/chunk/Mesh.h"
#include "world/chunk/ChunkManager.h"

namespace Jukcraft {
	static inline uint8_t smooth(uint8_t light, uint8_t b, uint8_t c, uint8_t d) {
		if (!(light && b && c && d)) {
			std::array<uint8_t, 4> l = {
				light,
				b > 0 ? b : std::numeric_limits<uint8_t>::max(),
				c > 0 ? c : std::numeric_limits<uint8_t>::max(),
				d > 0 ? d : std::numeric_limits<uint8_t>::max()
			};

			uint8_t min_val = *(std::min_element(l.begin(), l.end()));

			light = std::max(light, min_val);
			b = std::max(b, min_val);
			c = std::max(c, min_val);
			d = std::max(d, min_val);
			
			
			
		}
		return light + b + c + d; // To divide by 4

	}

	static inline uint8_t ao(bool s1, bool s2, bool c) {
		if (s1 && s2)
			return 1;

		return 4 - (s1 + s2 + c); // To divide by 4
	}

	Mesh::Mesh(size_t size, Chunk& chunk)
		:size(size), chunk(chunk) {
		vbo.allocate(CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 24, nullptr);

		vao.bindLayout(gfx::VertexArrayLayout{
			{
			   { 1, false },
			   { 1, false }
			}
			});
		vao.bindVertexBuffer(vbo.getTargetBuffer(), 0, gfx::VertexBufferLayout{
			{{ 0, 0 }, { 1, offsetof(VertexData, smoothLightData)}},
			0,
			sizeof(VertexData)
			});
		vao.bindIndexBuffer(Renderer::GetChunkIbo());

		icbo.allocate(sizeof(DrawIndirectCommand), nullptr);

		quadCount = 0;
	}

	Mesh::~Mesh() {

	}

	std::array<uint8_t, 4> Mesh::getFaceAO(uint8_t s1, uint8_t s2, uint8_t s3,
										   uint8_t s4,			   uint8_t s5,
										   uint8_t s6, uint8_t s7, uint8_t s8) {
		uint8_t vertex1 = ao(s2, s4, s1);
		uint8_t vertex2 = ao(s4, s7, s6);
		uint8_t vertex3 = ao(s5, s7, s8);
		uint8_t vertex4 = ao(s2, s5, s3);
		return { vertex1, vertex2, vertex3, vertex4 };
	}
		

	std::array<uint8_t, 4> Mesh::getSmoothLighting(uint8_t light, uint8_t light1, uint8_t light2, uint8_t light3,
												                  uint8_t light4,			      uint8_t light5,
												                  uint8_t light6, uint8_t light7, uint8_t light8) {
		uint8_t vertex1 = smooth(light, light2, light4, light1);
		uint8_t vertex2 = smooth(light, light4, light7, light6);
		uint8_t vertex3 = smooth(light, light5, light7, light8);
		uint8_t vertex4 = smooth(light, light2, light5, light3);
		return { vertex1, vertex2, vertex3, vertex4 };
	}
		
	std::array<glm::ivec3, 8> Mesh::getNeighbourVoxels(const glm::ivec3& npos, uint8_t normalIndex){
		std::array<glm::ivec3, 8> neighbours;
		switch (normalIndex) {
		case EAST_INDEX:
			neighbours = {
				npos + IUP + ISOUTH,	npos + IUP,		npos + IUP + INORTH,
				npos + ISOUTH,							npos + INORTH,
				npos + IDOWN + ISOUTH,	npos + IDOWN,	npos + IDOWN + INORTH
			};
			break;
		case WEST_INDEX:
			neighbours = {
				npos + IUP + INORTH, npos + IUP, npos + IUP + ISOUTH,
				npos + INORTH,                 npos + ISOUTH,
				npos + IDOWN + INORTH, npos + IDOWN, npos + IDOWN + ISOUTH
			};
			break;
		case UP_INDEX:
			neighbours = {
				npos + ISOUTH + IEAST, npos + ISOUTH, npos + ISOUTH + IWEST,
				npos + IEAST,					   npos + IWEST,
				npos + INORTH + IEAST, npos + INORTH, npos + INORTH + IWEST
			};
			break;
		case DOWN_INDEX:
			neighbours = {
				npos + ISOUTH + IWEST, npos + ISOUTH, npos + ISOUTH + IEAST,
				npos + IWEST,                         npos + IEAST,
				npos + INORTH + IWEST, npos + INORTH, npos + INORTH + IEAST
			};
			break;
		case SOUTH_INDEX:
			neighbours = {
				npos + IUP + IWEST, npos + IUP, npos + IUP + IEAST,
				npos + IWEST,                 npos + IEAST,
				npos + IDOWN + IWEST, npos + IDOWN, npos + IDOWN + IEAST
			};
			break;
		case NORTH_INDEX:
			neighbours = {
				npos + IUP + IEAST, npos + IUP, npos + IUP + IWEST,
				npos + IEAST,                 npos + IWEST,
				npos + IDOWN + IEAST, npos + IDOWN, npos + IDOWN + IWEST
			};
			break;
		default:
			THROW_ERROR("Invalid normal index value > 5");
		}
		
		return neighbours;
	}

	BakedQuad Mesh::bakeCubeFace(const glm::ivec3& localPos, uint8_t normalIndex, uint8_t blocklight, uint8_t skylight) {
		BakedQuad bakedQuad(blocklight, skylight);

		glm::ivec3 npos = localPos + IDIRECTIONS[normalIndex];
		std::array<glm::ivec3, 8> neighbours = getNeighbourVoxels(npos, normalIndex);
		std::array<uint8_t, 8> neighbourOpacity, neighbourLights, neighbourSkyLights;

		for (uint8_t i = 0; i < 8; i++) {
			neighbourOpacity[i]		= chunk.getOpacitySafe(neighbours[i]);
			neighbourLights[i]		= chunk.getBlockLightSafe(neighbours[i]);
			neighbourSkyLights[i]	= chunk.getSkyLightSafe(neighbours[i]);
		}

		bakedQuad.ambientOcclusionData = getFaceAO(
			neighbourOpacity[0], neighbourOpacity[1], neighbourOpacity[2],
			neighbourOpacity[3],					  neighbourOpacity[4],
			neighbourOpacity[5], neighbourOpacity[6], neighbourOpacity[7]
		);

		bakedQuad.smoothBlockLightData = getSmoothLighting(blocklight,
			neighbourLights[0], neighbourLights[1], neighbourLights[2],
			neighbourLights[3],					    neighbourLights[4],
			neighbourLights[5], neighbourLights[6], neighbourLights[7]
		);
		bakedQuad.smoothSkyLightData = getSmoothLighting(skylight,
			neighbourSkyLights[0], neighbourSkyLights[1], neighbourSkyLights[2],
			neighbourSkyLights[3],						  neighbourSkyLights[4],
			neighbourSkyLights[5], neighbourSkyLights[6], neighbourSkyLights[7]
		);
		return bakedQuad;
	}
	void Mesh::begin() {
		vbo.beginEditRegion(0, (uint32_t)size);
		quadCount = 0;
	}

	void Mesh::pushCubeFace(const Quad& quad, const glm::uvec3& localPos, uint8_t textureID, const BakedQuad& bakedQuad) {
		int i = 0;
		for (const Vertex& vertex : quad.vertices) {
			uint32_t v = ((vertex.pos.y + localPos.y) << 22) | ((vertex.pos.x + localPos.x) << 17) | ((vertex.pos.z + localPos.z) << 12) | (vertex.texUV << 10) | (textureID << 2) | (vertex.shading);
			uint32_t s = (bakedQuad.smoothSkyLightData[i] << 9) | (bakedQuad.smoothBlockLightData[i] << 3) | (bakedQuad.ambientOcclusionData[i]);
			vbo.push({ v, s });
			i++;
		}
		quadCount++;
	}
	
	void Mesh::end() {
		vbo.endEditRegion();
		DrawIndirectCommand cmd;
		cmd.count = (uint32_t)quadCount * 6;
		cmd.instanceCount = 1;
		cmd.firstIndex = 0;
		cmd.baseVertex = 0;
		cmd.baseInstance = 0;
		icbo.beginEditRegion(0, 1);
		icbo.editRegion(0, 1, &cmd);
		icbo.endEditRegion();
	}

	void Mesh::draw() {
		Renderer::MultiDrawElementsIndirect(vao, icbo.getTargetBuffer());
	}


}