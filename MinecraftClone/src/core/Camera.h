#pragma once
#include "renderer/Renderer.h"


class Camera {
public:
	Camera(Shader& shader, const glm::vec3& position, float yaw, float pitch);
	void update(const float delta_time);
	void onMouseMove(float x, float y);

	constexpr const glm::vec3& getPos() const { return position; }
	constexpr float getYaw() const { return yaw; }
	constexpr float getPitch() const { return pitch; }
private:
	Buffer ubo;
	Shader& shader;
	glm::vec3 position;
	float yaw, pitch;

	glm::vec2 lastCursorPos;

	struct ShaderCameraData {
		glm::mat4 transform;
		glm::vec4 pos;
	}* mappedUbo;

	FORBID_COPY(Camera);
	FORBID_MOVE(Camera);
};