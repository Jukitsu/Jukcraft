#pragma once

namespace Jukcraft {
	constexpr glm::vec3 g = glm::vec3(0.0f, -32.0f, 0.0f) / (TICK_RATE * TICK_RATE);
	
	constexpr float JUMP_HEIGHT = 1.25f; // Changed in Minecraft 1.9 for 1.5f
	constexpr float WALK_SPEED = 4.317f / TICK_RATE;

	constexpr glm::vec3 FRICTION = glm::vec3(20.0f, 20.0f, 20.0f) / TICK_RATE;

	constexpr glm::vec3	DRAG_FLY = glm::vec3(5.0f, 5.0f, 5.0f) / TICK_RATE;
	constexpr glm::vec3	DRAG_JUMP = glm::vec3(1.8f, 0.0f, 1.8f) / TICK_RATE;
	constexpr glm::vec3	DRAG_FALL = glm::vec3(1.8f, 0.4f, 1.8f) / TICK_RATE;

	constexpr float DASH_SPEED = 24.0f / TICK_RATE;
}
