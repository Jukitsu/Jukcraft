#include "pch.h"
#include <glad/glad.h>
#include "core/App.h"

/* Force the program to use dedicated GPU instead of iGPUs */
#ifdef _MSC_VER
extern "C" {
	__declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace Jukcraft {

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
		float lastTime2 = lastTime;
		while (!window->shouldClose()) {
			const float time = static_cast<float>(glfwGetTime());
			const float deltaTime = time - lastTime;
			lastTime = time;

			game->tick(deltaTime);

			lastTime2 = lastTime;
			while (glfwGetTime() - lastTime < 1.0f / TICK_RATE) {
				const float time2 = static_cast<float>(glfwGetTime());
				const float deltaTime2 = time2 - lastTime2;
				lastTime2 = time2;
				game->renderNewFrame(deltaTime2);

				window->endFrame();
			}
				

			window->endFrame();
		}
	}
	void App::onKeyPress(int key) {
		if (key == GLFW_KEY_ESCAPE){
			if (mouseCaptured) {
				mouseCaptured = false;
				window->releaseMouse();
			}
		}
		if (!mouseCaptured)
			return;
		switch (key) {
		case GLFW_KEY_F6:
			game->speedTime();
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
}