#include "pch.h"
#include "core/Game.h"
#include <GLFW/glfw3.h>
#include "core/HitRay.h"

static constexpr const char* vert = R"(
#version 460 core

layout(location = 0) in uint a_VertexData;

layout(location = 0) out VS_OUT {
	vec3 v_TexCoords;
	float v_Shading;
} vs_Out;

const vec2 c_TexCoords[4] = vec2[4] (
	vec2(0.0f, 1.0f),
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f)
);

layout(std140, binding = 0) uniform u_Camera {
	mat4 u_CameraTransforms;
	vec4 u_CameraPos;
};

layout(location = 0) uniform vec3 u_ChunkPos;

void main(void) {
	vec3 pos = u_ChunkPos + vec3(a_VertexData >> 17 & 0x1F, a_VertexData >> 22, a_VertexData >> 12 & 0x1F);

	gl_Position = u_CameraTransforms * vec4(pos, 1.0f);

	vs_Out.v_TexCoords = vec3(c_TexCoords[a_VertexData >> 10 & 0x3], a_VertexData >> 2 & 0xFF);
	vs_Out.v_Shading = float((a_VertexData & 0x3) + 2) / 5.0f;
}
)";

static constexpr const char* frag = R"(
#version 460 core

layout(early_fragment_tests) in;

layout(location = 0) in VS_OUT {
	vec3 v_TexCoords;
	float v_Shading;
} fs_In;

layout(binding = 0) uniform sampler2DArray u_TextureArraySampler;

layout(location = 0) out vec4 fragColor;

void main(void) {
	fragColor = texture(u_TextureArraySampler, fs_In.v_TexCoords) * fs_In.v_Shading;
}
)";

Game::Game() 
	:shader(vert, frag), camera(shader, glm::vec3(5.0f, 70.0f, 10.0f), glm::pi<float>() / 2.0f, 0.0f), textureManager(16) {
	textureManager.pushSubTexture("assets/textures/stone.png");
	textureManager.pushSubTexture("assets/textures/grass.png");
	textureManager.pushSubTexture("assets/textures/grass_side.png");
	textureManager.pushSubTexture("assets/textures/dirt.png");
	textureManager.pushSubTexture("assets/textures/cobblestone.png");
	textureManager.pushSubTexture("assets/textures/planks.png");
	textureManager.setSamplerUnit(0);


	blocks.emplace_back("Air", 0, models.air, std::vector<uint8_t>{});
	blocks.emplace_back("Stone", 1, models.cube, std::vector<uint8_t>{0, 0, 0, 0, 0, 0});
	blocks.emplace_back("Grass", 2, models.cube, std::vector<uint8_t>{2, 2, 1, 3, 2, 2});
	blocks.emplace_back("Dirt", 3, models.cube, std::vector<uint8_t>{3, 3, 3, 3, 3, 3});
	blocks.emplace_back("Cobblestone", 4, models.cube, std::vector<uint8_t>{4, 4, 4, 4, 4, 4});
	blocks.emplace_back("Planks", 5, models.cube, std::vector<uint8_t>{5, 5, 5, 5, 5, 5});

	world = std::make_unique<World>(blocks, shader);
}

void Game::onMousePress(int button) {

	HitRay hitray(*world, camera);
	World& w = *world;
	while (hitray.distance < HIT_RANGE)
		if (hitray.step(button,
			std::bind(&Game::hitCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		)) {
			break;
		}
			
}
void Game::hitCallback(int button, const glm::vec3& currentBlock, const glm::vec3& nextBlock) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		world->setBlock(nextBlock, 0);
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		world->setBlock(currentBlock, holding);
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		holding = world->getBlock(nextBlock);
		break;
	}
}


void Game::tick(const float deltaTime) {
	world->tick();
	camera.update(deltaTime);

	Renderer::Begin();
	world->render();
	Renderer::End();
}