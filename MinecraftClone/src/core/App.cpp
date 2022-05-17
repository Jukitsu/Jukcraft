#include "pch.h"
#include <glad/glad.h>
#include "core/App.h"
 
App* App::Instance = nullptr;

static constexpr float vertices[] = {
	 0.0f,   0.5f,  0.0f,
	 0.5f,  -0.5f,  0.0f,
	-0.5f,  -0.5f,  0.0f,
};

static constexpr const char* vert = R"(
#version 460 core

layout(location = 0) in vec3 va_Pos;

void main(void) {
	gl_Position = vec4(va_Pos, 1.0f);
}
)";

static constexpr const char* frag = R"(
#version 460 core

layout(location = 0) out vec4 fragColor;

void main(void) {
	fragColor = vec4(1.0f);
}
)";
App::App() {
	if (!Instance)
		Instance = this;
	else
		ERROR("Cannot create app twice!");
	if (!glfwInit())
		ERROR("Failed to initialize glfw");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(852, 480, "Minecraft clone", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		ERROR("Failed to load GL");

	glCreateVertexArrays(1, &vao);
	
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertices), vertices, 0);

	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 3);
	glVertexArrayBindingDivisor(vao, 0, 0);

	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

	program = glCreateProgram();

	GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsh, 1, &vert, nullptr);
	glCompileShader(vsh);

	GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsh, 1, &frag, nullptr);
	glCompileShader(fsh);

	glAttachShader(program, vsh);
	glAttachShader(program, fsh);
	glLinkProgram(program);
}

App::~App() {

}

void App::run() {
	running = true;
	while (running) {
		running = !glfwWindowShouldClose(window);

		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}