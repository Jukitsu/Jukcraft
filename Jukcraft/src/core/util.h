#pragma once


#define FORBID_COPY(CLS) CLS(const CLS&) = delete; CLS& operator=(const CLS&) = delete
#define FORBID_MOVE(CLS) CLS(CLS&&) noexcept = delete; CLS& operator=(CLS&&) noexcept = delete

#ifdef _DEBUG
#define DEBUGBREAK() std::abort()
#else
#define DEBUGBREAK()
#endif

#define THROW_ERROR(...) { LOG_ERROR(__VA_ARGS__); DEBUGBREAK(); }
#define CHECK(x, ...) if (!(x)) THROW_ERROR(__VA_ARGS__);

namespace Jukcraft {

	

	struct Vertex {
		glm::uvec3 pos;
		uint32_t texUV;
		uint32_t texIndex;
		uint32_t shading;
	};

	struct Quad {
		std::array<Vertex, 4> vertices;
	};

	struct VertexData {
		uint32_t vertexData;
		uint32_t smoothLightData;
	};
	
	

	struct ShaderCameraData {
		glm::mat4 transform;
		glm::vec4 pos;
	};

	struct DrawIndirectCommand {
		uint32_t  count;
		uint32_t  instanceCount;
		uint32_t  firstIndex;
		uint32_t  baseVertex;
		uint32_t  baseInstance;
	};

	constexpr uint8_t CHUNK_DIM = 16;
	constexpr uint8_t CHUNK_HEIGHT = 128;

	constexpr uint8_t RENDER_REGION_SIZE = 2;

	constexpr size_t MAX_QUADS = RENDER_REGION_SIZE * CHUNK_DIM * CHUNK_DIM * CHUNK_HEIGHT * 6;

	constexpr uint8_t WORLD_SIZE = 8;

	struct PerChunkData {
		glm::vec3 chunkPos;
	};

	struct BlockPos : public glm::ivec3 {

		BlockPos() :glm::ivec3() {}
		BlockPos(const glm::ivec3& vec) :glm::ivec3() {}
		BlockPos(glm::ivec3&& vec) :glm::ivec3(vec) {}
		BlockPos(int x, int y, int z) :glm::ivec3(x, y, z) {}
		BlockPos(int scalar) :glm::ivec3(scalar) {}
		BlockPos(const glm::ivec2& chunkPos, const glm::ivec3& localPos) 
			:glm::ivec3((int)CHUNK_DIM * glm::ivec3(chunkPos.x, 0, chunkPos.y) + localPos)
		{
			
		}

		constexpr glm::ivec2 getChunkPos() const noexcept {
			return { glm::floor((float)x / CHUNK_DIM), glm::floor((float)z / CHUNK_DIM) };
		}

		constexpr glm::ivec3 getLocalPos() const noexcept {
			return { glm::mod((float)x, (float)CHUNK_DIM), y, glm::mod((float)z, (float)CHUNK_DIM) };
		}

	};



	constexpr glm::vec3 EAST = glm::vec3(1.0f, 0.0f, 0.0f);
	constexpr glm::vec3 WEST = glm::vec3(-1.0f, 0.0f, 0.0f);
	constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
	constexpr glm::vec3 DOWN = glm::vec3(0.0f, -1.0f, 0.0f);
	constexpr glm::vec3 SOUTH = glm::vec3(0.0f, 0.0f, 1.0f);
	constexpr glm::vec3 NORTH = glm::vec3(0.0f, 0.0f, -1.0f);

	constexpr float TICK_RATE = 128.0f;

	constexpr uint8_t EAST_INDEX  = 0;
	constexpr uint8_t WEST_INDEX  = 1;
	constexpr uint8_t UP_INDEX    = 2;
	constexpr uint8_t DOWN_INDEX  = 3;
	constexpr uint8_t SOUTH_INDEX = 4;
	constexpr uint8_t NORTH_INDEX = 5;

	constexpr glm::vec3 DIRECTIONS[6] = {
		EAST, WEST, UP, DOWN, SOUTH, NORTH
	};

	constexpr glm::ivec3 IEAST = glm::ivec3(1, 0, 0);
	constexpr glm::ivec3 IWEST = glm::ivec3(-1, 0, 0);
	constexpr glm::ivec3 IUP = glm::ivec3(0, 1, 0);
	constexpr glm::ivec3 IDOWN = glm::ivec3(0, -1, 0);
	constexpr glm::ivec3 ISOUTH = glm::ivec3(0, 0, 1);
	constexpr glm::ivec3 INORTH = glm::ivec3(0, 0, -1);

	constexpr glm::ivec2 HEAST(1, 0);
	constexpr glm::ivec2 HWEST(-1, 0);
	constexpr glm::ivec2 HSOUTH(0, 1);
	constexpr glm::ivec2 HNORTH(0, -1);

	constexpr glm::ivec3 IDIRECTIONS[6] = {
		IEAST, IWEST, IUP, IDOWN, ISOUTH, INORTH
	};

	constexpr glm::ivec2 HAXIS[4] = {
		HEAST, HWEST, HSOUTH, HNORTH
	};

	using BlockID = uint8_t;

	template<typename T>
	using Auto = std::unique_ptr<T>;

	template<typename T>
	using Shared = std::shared_ptr<T>;

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	template<typename T>
	using Nullable = std::optional<T>;


	[[nodiscard]] inline std::string parseFile(const std::string_view& filepath) {
		std::ifstream stream(filepath.data(), std::ios::in);
		std::string source;
		CHECK(stream, "Could not open the file!");

		stream.seekg(0, std::ios::end);
		source.resize(stream.tellg());
		stream.seekg(0, std::ios::beg);
		stream.read(&source[0], source.size());
		stream.close();
		return source;
	}

	template<typename T>
	constexpr int sign(T x) {
		return (x > 0) - (x <= 0);
	}

	template<typename T>
	constexpr T wrapRadians(T x) {
		T y = glm::mod<T>(x, 2.0f * glm::pi<T>());
		return y > glm::pi<T>() ? y - 2.0f * glm::pi<T>() : y;
	}
}