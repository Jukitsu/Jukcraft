#pragma once
#include <GLFW/glfw3.h>

class App {
public:
	App();
	~App();

	void run();
private:
	static App* Instance;
	bool running = false;
	GLFWwindow* window;
	GLuint vao, vbo, program;

	FORBID_COPY(App);
	FORBID_MOVE(App);
};