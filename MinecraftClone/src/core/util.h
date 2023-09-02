#pragma once


#define FORBID_COPY(CLS) CLS(const CLS&) = delete; CLS& operator=(const CLS&) = delete
#define FORBID_MOVE(CLS) CLS(CLS&&) noexcept = delete; CLS& operator=(CLS&&) noexcept = delete

#ifdef _DEBUG
#define DEBUGBREAK() __debugbreak()
#else
#define DEBUGBREAK()
#endif

#define THROW_ERROR(...) { LOG_ERROR(__VA_ARGS__); DEBUGBREAK(); }
#define CHECK(x, ...) if (!(x)) THROW_ERROR(__VA_ARGS__);

struct Vertex {
	glm::uvec3 pos;
	uint32_t texUV;
	uint32_t texIndex;
	uint32_t shading;
};

struct Quad {
	std::array<Vertex, 4> vertices;
};

struct DrawIndirectCommand {
	uint32_t  count;
	uint32_t  instanceCount;
	uint32_t  firstIndex;
	uint32_t  baseVertex;
	uint32_t  baseInstance;
};

constexpr glm::vec3 EAST	= glm::vec3( 1.0f,  0.0f,  0.0f);
constexpr glm::vec3 WEST	= glm::vec3(-1.0f,  0.0f,  0.0f);
constexpr glm::vec3 UP		= glm::vec3( 0.0f,  1.0f,  0.0f);
constexpr glm::vec3 DOWN	= glm::vec3( 0.0f, -1.0f,  0.0f);
constexpr glm::vec3 SOUTH	= glm::vec3( 0.0f,  0.0f,  1.0f);
constexpr glm::vec3 NORTH	= glm::vec3( 0.0f,  0.0f, -1.0f);

constexpr glm::vec3 DIRECTIONS[6] = {
	EAST, WEST, UP, DOWN, SOUTH, NORTH
};

constexpr glm::ivec3 IEAST = glm::ivec3(1, 0, 0);
constexpr glm::ivec3 IWEST = glm::ivec3(-1, 0, 0);
constexpr glm::ivec3 IUP = glm::ivec3(0, 1, 0);
constexpr glm::ivec3 IDOWN = glm::ivec3(0, -1, 0);
constexpr glm::ivec3 ISOUTH = glm::ivec3(0, 0, 1);
constexpr glm::ivec3 INORTH = glm::ivec3(0, 0, -1);

constexpr glm::ivec3 IDIRECTIONS[6] = {
	IEAST, IWEST, IUP, IDOWN, ISOUTH, INORTH
};
using BlockID = uint8_t;


[[nodiscard]] inline std::string parseFile(const std::string_view& filepath) {
	std::ifstream stream(filepath, std::ios::in, std::ios::binary);
	std::string source;
	CHECK(stream, "Could not open the file!");

	stream.seekg(0, std::ios::end);
	source.resize(stream.tellg());
	stream.seekg(0, std::ios::beg);
	stream.read(&source[0], source.size());
	stream.close();
	return source;
}