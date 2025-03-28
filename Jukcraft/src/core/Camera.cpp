#include "pch.h"
#include "Camera.h"

#include <GLFW/glfw3.h>
#include "core/App.h"

static constexpr float sensitivity = 0.002f;

namespace Jukcraft {
	Camera::Camera(gfx::Shader& shader, Player& player)
		:shader(shader), player(player), speed(WALK_SPEED) {
		ubo.allocate(sizeof(ShaderCameraData), nullptr, true);
		mappedUbo = reinterpret_cast<ShaderCameraData*>(ubo.map(0, sizeof(ShaderCameraData)));

		ubo.bindRange(gfx::BufferBindingTarget::Uniform, 0, 0, sizeof(ShaderCameraData));
		lastCursorPos = App::GetWindow().getCursorPos();

	}


	void Camera::update(const float partialTicks) {
		isFirstPerson = isFirstPerson && player.getDeathTime() <= 0;
		glm::vec3 interpolatedPos = glm::mix(player.old.position, player.position, partialTicks);

		if (!isFirstPerson)
			interpolatedPos -= 4.0f * player.getEyesight();

		const glm::vec2 cursorPos = App::GetWindow().getCursorPos();
		const glm::vec2 deltaCursor = cursorPos - lastCursorPos;
		lastCursorPos = cursorPos;

		if (!App::Get().isMouseCaptured())
			return;

		player.setYaw(player.getYaw() + deltaCursor.x * sensitivity);
		player.setPitch(player.getPitch() - deltaCursor.y * sensitivity);
		// It is negative because the origin is in the top left corner, not bottom left		

		glm::ivec3 input(0);
		if (App::GetWindow().isKeyPressed(GLFW_KEY_W))			input.z += 1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_S))			input.z += -1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_A))			input.x += -1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_D))			input.x += 1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_LEFT_SHIFT)) input.y += -1;
		if (App::GetWindow().isKeyPressed(GLFW_KEY_SPACE))		input.y += 1;


		if (input.y > 0) {
			player.setJumping(true);
		} 

		player.setInput(glm::vec3((float)input.x, 0.0f, (float)input.z));

		
		glm::mat4 proj = glm::perspective(
			isFirstPerson ? glm::mix(player.fovOld, player.fov, partialTicks) : glm::radians(90.0f),
			static_cast<float>(App::GetWindow().getWidth()) / App::GetWindow().getHeight(),
			0.1f,
			500.0f
		);


		glm::mat4 view = glm::mat4(1.0f);

		if (player.isHurt()) {
			view = glm::rotate(view, (player.iframes - partialTicks) * glm::pi<float>() / 200, NORTH);
		}

		view = glm::rotate(view, player.getPitch(), WEST);
		view = glm::rotate(view, player.getYaw() + glm::pi<float>() / 2, UP);

		view = glm::translate(view, -interpolatedPos - glm::vec3(0, player.getEyeLevel(), 0));
		

		mappedUbo->transform = proj * view;

		mappedUbo->pos = glm::vec4(player.position, 1.0f);
		ubo.flush(0, sizeof(ShaderCameraData));
	}
}
