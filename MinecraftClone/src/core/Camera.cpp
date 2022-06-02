#include "pch.h"
#include "Camera.h"

#include <GLFW/glfw3.h>
#include "core/App.h"

static constexpr float speed = 7.0f;
static constexpr float sensitivity = 0.001f;

Camera::Camera(Shader& shader, const glm::vec3& position, float yaw, float pitch)
	:shader(shader), position(position), yaw(yaw), pitch(pitch) {
	ubo.allocate(sizeof(ShaderCameraData), nullptr, true);
	mappedUbo = reinterpret_cast<ShaderCameraData*>(ubo.map(0, sizeof(ShaderCameraData)));

	ubo.bindRange(BufferBindingTarget::Uniform, 0, 0, sizeof(ShaderCameraData));
	lastCursorPos = App::GetWindow().getCursorPos();
}

void Camera::update(float delta_time) {
	bool moved = false;

	glm::vec2 cursorPos = App::GetWindow().getCursorPos();
	glm::vec2 deltaCursor = cursorPos - lastCursorPos;
	lastCursorPos = cursorPos;

	if (cursorPos.x && cursorPos.y)
		moved = true;

	yaw = glm::mod(yaw + deltaCursor.x * sensitivity, 2.0f * glm::pi<float>());
	pitch = glm::clamp(pitch + deltaCursor.y * sensitivity, -glm::pi<float>() / 2, glm::pi<float>() / 2);

	if (!App::Get().isMouseCaptured())
		return;
	
	glm::ivec3 input(0);
	if (App::GetWindow().isKeyPressed(GLFW_KEY_W))			input.z += -1;
	if (App::GetWindow().isKeyPressed(GLFW_KEY_S))			input.z +=  1;
	if (App::GetWindow().isKeyPressed(GLFW_KEY_A))			input.x += -1;
	if (App::GetWindow().isKeyPressed(GLFW_KEY_D))			input.x +=  1;
	if (App::GetWindow().isKeyPressed(GLFW_KEY_LEFT_SHIFT)) input.y += -1;
	if (App::GetWindow().isKeyPressed(GLFW_KEY_SPACE))		input.y +=  1;


	const float multiplier = speed * delta_time;
	if (input.y) {
		position.y += input.y * multiplier;
		moved = true;
	} if (input.x || input.z) {
		const float angle = yaw + std::atan2(input.z, input.x) - glm::pi<float>() / 2;
		position.x += glm::cos(angle) * multiplier;
		position.z += glm::sin(angle) * multiplier;
		moved = true;
	}

	if (!moved)
		return;

	glm::mat4 proj = glm::perspective(
		90.0f,
		static_cast<float>(App::GetWindow().getWidth()) / App::GetWindow().getHeight(),
		0.1f,
		500.0f
	);
	glm::mat4 view = glm::translate(
		glm::rotate(
			glm::rotate(
				glm::mat4(1.0f),
				-pitch,
				-glm::vec3(1.0f, 0.0f, 0.0f)
			),
			-(yaw - glm::pi<float>() / 2),
			-glm::vec3(0.0f, 1.0f, 0.0f)
		),
		-position
	);
	mappedUbo->transform = proj * view;
	
	mappedUbo->pos = glm::vec4(position, 1.0f);
	ubo.flush(0, sizeof(ShaderCameraData));
}