#include "pch.h"
#include <glad/glad.h>
#include "core/App.h"

App* App::Instance = nullptr;

App::App() {
	if (!Instance) {
		Instance = this;
		Log::Init();
	}
	else {
		THROW_ERROR("Cannot create app twice!");
	}


	if (!glfwInit())
		THROW_ERROR("Failed to initialize glfw");

	EventCallbacks callbacks;
	callbacks.keyPressCallback = std::bind(&App::onKeyPress, this, std::placeholders::_1);
	callbacks.mousePressCallback = std::bind(&App::onMousePress, this, std::placeholders::_1);
	callbacks.resizeCallback = std::bind(&App::onResize, this, std::placeholders::_1, std::placeholders::_2);
	window.emplace(852, 480, callbacks);

	Renderer::Init();
	
	game = std::make_shared<Game>();
}

App::~App() {

}

void App::run() {
	float lastTime = static_cast<float>(glfwGetTime());
	while (!window->shouldClose()) {
		const float time = static_cast<float>(glfwGetTime());
		const float deltaTime = time - lastTime;
		lastTime = time;

		game->tick(deltaTime);

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
	if (!mouseCaptured && button == GLFW_MOUSE_BUTTON_LEFT) {
		mouseCaptured = true;
		window->captureMouse();
		return;
	}

	game->onMousePress(button);
}


void App::onResize(uint16_t width, uint16_t height) {
	Renderer::Viewport(width, height);
}