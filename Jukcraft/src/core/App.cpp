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

		float lastTime = static_cast<float>(glfwGetTime()), timer = lastTime;
		float partialTicks = 0.0f, nowTime = 0.0f;
		int frames = 0, updates = 0;

		float deltaTime = 0.0f;
		while (!window->shouldClose()) {

			nowTime = static_cast<float>(glfwGetTime());
			partialTicks += (nowTime - lastTime) * TICK_RATE;
			lastTime = nowTime;

			// - Only update at 64 tick / s
			while (partialTicks >= 1.0) {
				game->tick();   // - Update function
				updates++;
				partialTicks--;

			}

			game->renderNewFrame(partialTicks);
			frames++;

			window->endFrame();

			if (glfwGetTime() - timer > 1.0) {
				timer++;
				LOG_TRACE("FPS: {}, TPS: {}", frames, updates);
				updates = 0, frames = 0;
			}
			
			
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