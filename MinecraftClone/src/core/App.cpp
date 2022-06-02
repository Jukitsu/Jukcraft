#include "pch.h"
#include <glad/glad.h>
#include "core/App.h"

App* App::Instance = nullptr;

static constexpr std::array<int, 243> vertices = {
	1 , 1 , 1 ,		0 , 1 , 0 ,		1 , 0 , 0 ,
	1 , 0 , 1 ,		0 , 0 , 0 ,		1 , 0 , 0 ,
	1 , 0 , 0 ,		1 , 0 , 0 ,		1 , 0 , 0 ,
	1 , 1 , 0 ,		1 , 1 , 0 ,		1 , 0 , 0 ,

	0 , 1 , 0 ,		0 , 1 , 0 ,		-1 , 0 , 0 ,
	0 , 0 , 0 ,		0 , 0 , 0 ,		-1 , 0 , 0 ,
	0 , 0 , 1 ,		1 , 0 , 0 ,		-1 , 0 , 0 ,
	0 , 1 , 1 ,		1 , 1 , 0 ,		-1 , 0 , 0 ,

	1 ,  1 ,  1 ,	0 , 1 , 0 ,		0 , 1 , 0 ,
	1 ,  1 ,  0 ,	0 , 0 , 0 ,		0 , 1 , 0 ,
	0 ,  1 ,  0 ,	1 , 0 , 0 ,		0 , 1 , 0 ,
	0 ,  1 ,  1 ,	1 , 1 , 0 ,		0 , 1 , 0 ,

	0 , 0 , 1 ,		0 , 1 , 0 ,		0 , -1 , 0 ,
	0 , 0 , 0 ,		0 , 0 , 0 ,		0 , -1 , 0 ,
	1 , 0 , 0 ,		1 , 0 , 0 ,		0 , -1 , 0 ,
	1 , 0 , 1 ,		1 , 1 , 0 ,		0 , -1 , 0 ,

	0 ,  1 ,  1 ,	0 , 1 , 0 ,		0 , 0 , 1 ,
	0 ,  0 ,  1 ,	0 , 0 , 0 ,		0 , 0 , 1 ,
	1 ,  0 ,  1 ,	1 , 0 , 0 ,		0 , 0 , 1 ,
	1 ,  1 ,  1 ,	1 , 1 , 0 ,		0 , 0 , 1 ,

	1 ,  1 , 0 ,	0 , 1 , 0 ,		0 , 0 , -1 ,
	1 ,  0 , 0 ,	0 , 0 , 0 ,		0 , 0 , -1 ,
	0 ,  0 , 0 ,	1 , 0 , 0 ,		0 , 0 , -1 ,
	0 ,  1 , 0 ,	1 , 1 , 0 ,		0 , 0 , -1 
};

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

layout(location = 0) in ivec3 va_Pos;
layout(location = 1) in ivec3 va_TexCoords;
layout(location = 2) in ivec3 va_Normal;

out VS_OUT {
	vec3 v_TexCoords;
} vs_Out;

layout(std140, binding = 0) uniform u_Camera {
	mat4 u_CameraTransforms;
	vec4 u_CameraPos;
};
void main(void) {
	gl_Position = u_CameraTransforms * vec4(va_Pos, 1.0f);

	vs_Out.v_TexCoords = vec3(va_TexCoords);
}
)";

static constexpr const char* frag = R"(
#version 460 core

in VS_OUT {
	vec3 v_TexCoords;
} fs_In;

layout(binding = 0) uniform sampler2DArray u_TextureArraySampler;

layout(location = 0) out vec4 fragColor;

void main(void) {
	fragColor = texture(u_TextureArraySampler, fs_In.v_TexCoords);
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

	vbo->allocate(sizeof(vertices), vertices.data(), false);
	ibo->allocate(sizeof(indices), indices.data(), false);

	vao->bindLayout(VertexArrayLayout{
		{ 
		   { 3, false },
		   { 3, false },
		   { 3, false },
		}
	});
	vao->bindVertexBuffer(*vbo, 0, VertexBufferLayout{
		{{ 0 }, { 1, 3 * sizeof(int) }, { 2, 6 * sizeof(int) }},
		0,
		9 * sizeof(int)
		});
	vao->bindIndexBuffer(*ibo);

	textureManager.emplace(16);
	textureManager->loadSubTexture(0, "assets/textures/cobblestone.png");
	textureManager->setSamplerUnit(0);

	shader.emplace(vert, frag);

	camera.emplace(*shader, glm::vec3(0.0f), 0.0f, 0.0f);
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