#include "pch.h"
#include <glad/glad.h>
#include "core/App.h"

App* App::Instance = nullptr;

struct Vertex {
	glm::uvec3 pos;
	uint32_t texUV;
	uint32_t texIndex;
	uint32_t shading;
};

static constexpr std::array<Vertex, 24> cube = {
	Vertex { { 1, 1, 1 },		0,	0,	1 },
	Vertex { { 1, 0, 1 },		1,	0,	1 },
	Vertex { { 1, 0, 0 },		2,	0,	1 },
	Vertex { { 1, 1, 0 },		3,	0,	1 },
						 		 					  
	Vertex { { 0, 1, 0 },		0,	0,	1 },
	Vertex { { 0, 0, 0 },		1,	0,	1 },
	Vertex { { 0, 0, 1 },		2,	0,	1 },
	Vertex { { 0, 1, 1 },		3,	0,	1 },
								 					  
	Vertex { { 1, 1, 1 },		0,	0,	3 },
	Vertex { { 1, 1, 0 },		1,	0,	3 },
	Vertex { { 0, 1, 0 },		2,	0,	3 },
	Vertex { { 0, 1, 1 },		3,	0,	3 },
							 						  
	Vertex { { 0, 0, 1 },		0,	0,	0 },
	Vertex { { 0, 0, 0 },		1,	0,	0 },
	Vertex { { 1, 0, 0 },		2,	0,	0 },
	Vertex { { 1, 0, 1 },		3,	0,	0 },
							 						  
	Vertex { { 0, 1, 1 },		0,	0,	2 },
	Vertex { { 0, 0, 1 },		1,	0,	2 },
	Vertex { { 1, 0, 1 },		2,	0,	2 },
	Vertex { { 1, 1, 1 },		3,	0,	2 },
 													  
	Vertex { { 1, 1, 0 },		0,	0,	2 },
	Vertex { { 1, 0, 0 },		1,	0,	2 },
	Vertex { { 0, 0, 0 },		2,	0,	2 },
	Vertex { { 0, 1, 0 },		3,	0,	2 }
};

std::vector<uint32_t> getCompactCube() {
	std::vector<uint32_t> result;
	result.reserve(cube.size());
	for (const Vertex& vertex : cube) {
		uint32_t v = (vertex.pos.x << 20) | (vertex.pos.y << 16) | (vertex.pos.z << 12) | (vertex.texUV << 10) | (vertex.texIndex << 2) | (vertex.shading);
		result.push_back(v);
	}
	return result;
}

static constexpr std::array<uint32_t, 36> indices = {
	 0,  1,  2,  0,  2,  3,
	 4,  5,  6,  4,  6,  7,
	 8,  9, 10,  8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23,
};


static constexpr const char* vert = R"(
#version 460 core

layout(location = 0) in uint a_VertexData;

out VS_OUT {
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
void main(void) {
	vec3 pos = vec3((a_VertexData >> 20) & 0xF, (a_VertexData >> 16) & 0xF, (a_VertexData >> 12) & 0xF);

	gl_Position = u_CameraTransforms * vec4(pos, 1.0f);

	vs_Out.v_TexCoords = vec3(c_TexCoords[(a_VertexData >> 10) & 0x3], (a_VertexData >> 2) & 0xFF);
	vs_Out.v_Shading = float((a_VertexData & 0x3) + 2) / 5.0f;
}
)";

static constexpr const char* frag = R"(
#version 460 core

in VS_OUT {
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

	vao.emplace();
	vbo.emplace();
	ibo.emplace();

	std::vector<uint32_t> vertices = getCompactCube();

	vbo->allocate(vertices.size() * sizeof(uint32_t), vertices.data(), false);
	ibo->allocate(sizeof(indices), indices.data(), false);

	vao->bindLayout(VertexArrayLayout{
		{ 
		   { 1, false },
		}
	});
	vao->bindVertexBuffer(*vbo, 0, VertexBufferLayout{
		{{ 0 }},
		0,
		sizeof(uint32_t)
		});
	vao->bindIndexBuffer(*ibo);

	textureManager.emplace(16);
	textureManager->loadSubTexture(0, "assets/textures/cobblestone.png");
	textureManager->setSamplerUnit(0);

	shader.emplace(vert, frag);

	camera.emplace(*shader, glm::vec3(0.0f, 0.0f, 0.0f), -glm::pi<float>() / 2, 0.0f);
}

App::~App() {

}

void App::run() {
	float last_time = static_cast<float>(glfwGetTime());
	while (!window->shouldClose()) {
		const float time = static_cast<float>(glfwGetTime());
		const float delta_time = time - last_time;
		last_time = time;

		camera->update(delta_time);
		Renderer::Begin();
		//glClearNamedFramebufferfv(0, GL_COLOR, 0, glm::value_ptr(empty_color));
		shader->bind();
		Renderer::DrawElements(*vao, 36);
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