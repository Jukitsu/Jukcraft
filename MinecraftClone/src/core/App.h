#pragma once
#include <GLFW/glfw3.h>
#include "renderer/Renderer.h"
#include "core/Window.h"
#include "core/Game.h"


class App {
public:
	App();
	~App();

	void run();

	static App& Get() { return *Instance; }
	static Window& GetWindow() { return *Get().window; }

	void onKeyPress(int key);
	void onMousePress(int button);
	void onResize(uint16_t width, uint16_t height);

	constexpr bool isMouseCaptured() const { return mouseCaptured; }
private:
	static App* Instance;
	std::optional<Window> window;
	std::shared_ptr<Game> game;

	bool mouseCaptured = false;

	FORBID_COPY(App);
	FORBID_MOVE(App);
};