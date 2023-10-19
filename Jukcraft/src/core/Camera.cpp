#include "pch.h"
#include "Camera.h"

#include <GLFW/glfw3.h>
#include "core/App.h"

static constexpr float speed = 7.0f;
static constexpr float sensitivity = 0.005f;

namespace Jukcraft {
	Camera::Camera(gfx::Shader& shader, const glm::vec3& position, float yaw, float pitch)
		:shader(shader), position(position), yaw(yaw), pitch(pitch) {
		ubo.allocate(sizeof(ShaderCameraData), nullptr, true);
		mappedUbo = reinterpret_cast<ShaderCameraData*>(ubo.map(0, sizeof(ShaderCameraData)));

		ubo.bindRange(gfx::BufferBindingTarget::Uniform, 0, 0, sizeof(ShaderCameraData));
		lastCursorPos = App::GetWindow().getCursorPos();
	}


	void Camera::update(const float deltaTime) {
		const glm::vec2 cursorPos = App::GetWindow().getCursorPos();
		const glm::vec2 deltaCursor = cursorPos - lastCursorPos;
		lastCursorPos = cursorPos;


		yaw = yaw + deltaCursor.x * sensitivity, 2.0f * glm::pi<float>();
		pitch = glm::clamp(pitch - deltaCursor.y * sensitivity, -glm::pi<float>() / 2, glm::pi<float>() / 2);
		// It is negative because the origin is in the top left corner, not bottom left

		if (!App::Get().isMouseCaptured())
			return;

		glm::ivec3 input(0);
		if (App::GetWindow().isKeyPressed(GLFW_KEY_W))			input.z += 1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_S))			input.z += -1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_A))			input.x += -1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_D))			input.x += 1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_LEFT_SHIFT)) input.y += -1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_SPACE))		input.y += 1;


		const float multiplier = speed * deltaTime;
		if (input.y) {
			position.y += input.y * multiplier;
		} if (input.x || input.z) {
			const float angle = yaw - glm::atan<float>((float)input.z, (float)input.x) + glm::pi<float>() / 2;
			position.x += glm::cos(angle) * multiplier;
			position.z += glm::sin(angle) * multiplier;
		}


		glm::mat4 proj = glm::perspective(
			glm::radians(90.0f),
			static_cast<float>(App::GetWindow().getWidth()) / App::GetWindow().getHeight(),
			0.1f,
			500.0f
		);
		glm::mat4 view = glm::rotate(glm::mat4(1.0f), pitch, -glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, yaw + glm::pi<float>() / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate(view, -position);

		mappedUbo->transform = proj * view;

		mappedUbo->pos = glm::vec4(position, 1.0f);
		ubo.flush(0, sizeof(ShaderCameraData));
	}
}
