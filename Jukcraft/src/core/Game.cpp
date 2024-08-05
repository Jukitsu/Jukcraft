#include "pch.h"
#include "core/Game.h"
#include <GLFW/glfw3.h>
#include "entity/HitRay.h"

namespace Jukcraft {

	Game::Game()
		:shader("assets/shaders/terrain/vert.glsl", "assets/shaders/terrain/frag.glsl"), camera(shader, player), textureManager(16) {
		textureManager.pushSubTexture("assets/textures/stone.png");
		textureManager.pushSubTexture("assets/textures/grass.png");
		textureManager.pushSubTexture("assets/textures/grass_side.png");
		textureManager.pushSubTexture("assets/textures/dirt.png");
		textureManager.pushSubTexture("assets/textures/cobblestone.png");
		textureManager.pushSubTexture("assets/textures/planks.png");
		textureManager.pushSubTexture("assets/textures/leaves.png");
		textureManager.setSamplerUnit(0);


		blocks.emplace_back("Air", 0, models.air, std::vector<uint8_t>{}, true, 0);
		blocks.emplace_back("Stone", 1, models.cube, std::vector<uint8_t>{0, 0, 0, 0, 0, 0}, false, 0);
		blocks.emplace_back("Grass", 2, models.cube, std::vector<uint8_t>{2, 2, 1, 3, 2, 2}, false, 0);
		blocks.emplace_back("Dirt", 3, models.cube, std::vector<uint8_t>{3, 3, 3, 3, 3, 3}, false, 0);
		blocks.emplace_back("Cobblestone", 4, models.cube, std::vector<uint8_t>{4, 4, 4, 4, 4, 4}, false, 0);
		blocks.emplace_back("Planks", 5, models.cube, std::vector<uint8_t>{5, 5, 5, 5, 5, 5}, false, 0);
		blocks.emplace_back("Leaves", 6, models.cube, std::vector<uint8_t>{6, 6, 6, 6, 6, 6}, true, 14);

		world = std::make_unique<World>(blocks, shader);
		player = std::make_unique<Player>(*world, glm::vec3(15.0f, 70.0f, 20.0f), glm::vec3(0.0f), 
			glm::pi<float>() / 2.0f, 0.0f);
	}

	void Game::onMousePress(int button) {

		HitRay hitray(*world, *player);
		World& w = *world;
		while (hitray.distance < HIT_RANGE)
			if (hitray.step(button,
				std::bind(&Game::hitCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
			)) {
				break;
			}

	}


	void Game::onKeyPress(int key) {
		switch (key) {
		case GLFW_KEY_F6:
			speedTime();
			break;
		case GLFW_KEY_LEFT_CONTROL:
			player->dash();
			break;
		case GLFW_KEY_F5:
			camera.isFirstPerson = !camera.isFirstPerson;
			break;
		}
	}
	void Game::hitCallback(int button, const BlockPos& currentBlock, const BlockPos& nextBlock) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			world->setBlock(nextBlock, 0);
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			world->trySetBlock(*player, currentBlock, holding);
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			holding = world->getBlock(nextBlock);
			break;
		}
	}


	void Game::tick() {
		world->tick();	
		player->tick();
	}

	void Game::renderNewFrame(const float partialTicks) {
		camera.update(partialTicks);

		Renderer::Begin(world->getSkyColor());
		world->render(partialTicks);

		if (!camera.isFirstPerson) {
			world->mobRenderer.render(*player, partialTicks);
		}
		

		Renderer::End();
	}
}