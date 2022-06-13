#include "pch.h"
#include <glad/glad.h>
#include "core/App.h"

App* App::Instance = nullptr;



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

App::App() {
	if (!Instance)
		Instance = this;
	else
		ERROR("Cannot create app twice!");

	if (!glfwInit())
		ERROR("Failed to initialize glfw");

	EventCallbacks callbacks;
	callbacks.keyPressCallback = std::bind(&App::onKeyPress, this, std::placeholders::_1);
	callbacks.mousePressCallback = std::bind(&App::onMousePress, this, std::placeholders::_1);
	callbacks.resizeCallback = std::bind(&App::onResize, this, std::placeholders::_1, std::placeholders::_2);
	window.emplace(852, 480, callbacks);

	Renderer::Init();

	chunkManager.emplace();

	textureManager.emplace(16);
	textureManager->loadSubTexture(0, "assets/textures/cobblestone.png");
	textureManager->setSamplerUnit(0);

	shader.emplace(vert, frag);

	camera.emplace(*shader, glm::vec3(0.0f, 0.0f, -3.0f), -glm::pi<float>() / 2, 0.0f);
}

App::~App() {

}

void App::run() {
	float last_time = static_cast<float>(glfwGetTime());
	while (!window->shouldClose()) {
		const float time = static_cast<float>(glfwGetTime());
		const float delta_time = time - last_time;
		last_time = time;

		chunkManager->tick();
		camera->update(delta_time);

		Renderer::Begin();
		shader->bind();
		chunkManager->drawChunksCubeLayers(*shader);
		Renderer::End();

		window->endFrame();
	}
}
void App::onKeyPress(int key) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (mouseCaptured) {
			mouseCaptured = false;
			window->releaseMouse();
		}
		break;
	}
}

void App::onMousePress(int button) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (!mouseCaptured) {
			mouseCaptured = true;
			window->captureMouse();
		}
	}
}


void App::onResize(uint16_t width, uint16_t height) {
	Renderer::Viewport(width, height);
}