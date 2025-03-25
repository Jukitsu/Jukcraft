#pragma once
#include "renderer/Renderer.h"
#include "entity/player/Player.h"
namespace Jukcraft {

	class Camera {
	public:
		Camera(gfx::Shader& shader, Player& player);
		void update(const float delta_time);
		bool isFirstPerson = true;
	private:
		gfx::Buffer ubo;
		gfx::Shader& shader;

		glm::vec2 lastCursorPos;
		Player& player;		

		float speed;

		ShaderCameraData *mappedUbo;


		FORBID_COPY(Camera);
		FORBID_MOVE(Camera);
	};

}