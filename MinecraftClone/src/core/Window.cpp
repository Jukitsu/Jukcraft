#include "pch.h"
#include "core/Window.h"
#include <glad/glad.h>


static void APIENTRY GLDebugMsgCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* msg, const void* data) {
	const char* _severity;

	std::stringstream ss;
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		ss << "[GL DEBUG MESSAGE]: "
			<< "ID " << id << ", "
			<< "message of "
			<< "HIGH" << " severity: \n" << msg << '\n';
		ERROR(ss.str());
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		ss << "[GL DEBUG MESSAGE]: "
			<< "ID " << id << ", "
			<< "message of "
			<< "Medium" << " severity: \n" << msg << '\n';
		std::cout << (ss.str());
		break;

	case GL_DEBUG_SEVERITY_LOW:
		ss << "[GL DEBUG MESSAGE]: "
			<< "ID " << id << ", "
			<< "message of "
			<< "Low" << " severity: \n" << msg << '\n';
		std::cout << (ss.str());
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		ss << "[GL DEBUG MESSAGE]: Notifcation: \n" << msg << '\n';
		std::cout << (ss.str());
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

}
static void GLFWErrorCallback(int error, const char* info) {
	ERROR("GLFW Error " << error << " " << info);
}

Window::Window(uint16_t width, uint16_t height, EventCallbacks callbacks)
	:width(width), height(height), callbacks(callbacks) {
	glfwSetErrorCallback(GLFWErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	handle = glfwCreateWindow(width, height, "Minecraft clone", nullptr, nullptr);

	glfwMakeContextCurrent(handle);
	glfwSetWindowUserPointer(handle, this);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		ERROR("Failed to load GL");
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLDebugMsgCallback, nullptr);
#endif
	glfwSetWindowSizeCallback(handle, [](GLFWwindow* window, int width, int height) {
		((Window*)glfwGetWindowUserPointer(window))->onResize(width, height);
		});
	glfwSetMouseButtonCallback(handle, [](GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS)
			((Window*)glfwGetWindowUserPointer(window))->getEventCallbacks().mousePressCallback(button);
		});
	glfwSetKeyCallback(handle, [](GLFWwindow* window, int key, int scan, int action, int mods) {
		if (action == GLFW_PRESS)
			((Window*)glfwGetWindowUserPointer(window))->getEventCallbacks().keyPressCallback(key);
		});
	glfwSwapInterval(0);
}