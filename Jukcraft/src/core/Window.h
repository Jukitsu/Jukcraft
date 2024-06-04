#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Jukcraft {

	struct EventCallbacks {
		std::function<void(int)> keyPressCallback; // App::onKeyPress
		std::function<void(int)> mousePressCallback; //  App::onMousePress
		std::function<void(uint16_t, uint16_t)> resizeCallback; // App::onResize
	};

	class Window {
	public:
		Window(uint16_t width, uint16_t height, EventCallbacks callbacks);
		~Window() {
			glfwDestroyWindow(handle);
		}
		void onResize(uint16_t width, uint16_t height) {
			this->width = width;
			this->height = height;
			callbacks.resizeCallback(width, height);
		}
		void endFrame() {
			glfwSwapBuffers(handle);
			glfwPollEvents();
		}
		bool shouldClose() const {
			return glfwWindowShouldClose(handle);
		}

		bool isKeyPressed(int key) const {
			return glfwGetKey(handle, key) == GLFW_PRESS;
		}

		bool isMousePressed(int mouse_button) const {
			return glfwGetMouseButton(handle, mouse_button) == GLFW_PRESS;
		}

		glm::vec2 getCursorPos() {
			double x, y;
			glfwGetCursorPos(handle, &x, &y);
			return { (float)x, (float)y };
		}
		constexpr uint16_t getWidth() const { return width; }
		constexpr uint16_t getHeight() const { return height; }

		void captureMouse() {
			glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		void releaseMouse() {
			glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
		constexpr EventCallbacks& getEventCallbacks() {
			return callbacks;
		}
	private:
		GLFWwindow* handle;
		uint16_t width, height;
		FORBID_COPY(Window);
		FORBID_MOVE(Window);
		EventCallbacks callbacks;
		friend struct SharedContext;
	};

	struct SharedContext {
		GLFWwindow* context;
		Window& window;

		SharedContext(Window& window) :window(window) {
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			context = glfwCreateWindow(window.getWidth(), window.getHeight(), "New Context", nullptr, window.handle);
		}
		~SharedContext() {
			glfwMakeContextCurrent(window.handle);
			glfwDestroyWindow(context);
		}

		void makeCurrent() {
			glfwMakeContextCurrent(context);
		}
	};

}