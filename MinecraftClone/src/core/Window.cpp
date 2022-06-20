#include "pch.h"
#include "core/Window.h"
#include <glad/glad.h>


static void APIENTRY GLDebugMsgCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* msg, const void* data) {
	const char* _severity;

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		THROW_ERROR("[GL ERROR] ID {} msg of HIGH severity: {}", id, msg);
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		LOG_WARN("[GL WARNING] ID {} msg of MEDIUM severity: {}", id, msg);
		break;

	case GL_DEBUG_SEVERITY_LOW:
		LOG_INFO("[GL INFO] ID {} msg of LOW severity : {}", id, msg);
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		LOG_TRACE("[GL DEBUG MESSAGE]: Notifcation: {}", msg);
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

}
static void GLFWErrorCallback(int error, const char* info) {
	THROW_ERROR("GLFW Error {} {}", error, info);
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
		THROW_ERROR("Failed to load GL");
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