#pragma once
#include "core/Camera.h"
#include "world/World.h"
#include "blocks/Block.h"
#include "entity/player/Player.h"

namespace Jukcraft {

	class Game {
	public:
		Game();
		void hitCallback(int button, const BlockPos& currentBlock, const BlockPos& nextBlock);
		void tick();
		void renderNewFrame(const float deltaTime);
		void onMousePress(int button);
		void onKeyPress(int key);
		void speedTime() {
			if (inWorld) {
				world->speedTime();
			}
		}
	private:
		gfx::Shader shader;
		TextureManager textureManager;
		Camera camera;
		Auto<World> world;
		BlockID holding = 6;
		bool inWorld = true;

		Auto<Player> player;

		std::vector<Block> blocks;
	};

}