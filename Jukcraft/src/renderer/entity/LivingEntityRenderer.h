#pragma once
#include "models/entity/EntityModel.h"
#include "entity/LivingEntity.h"
#include "renderer/Renderer.h"
#include "renderer/gfx/objects/VertexArray.h"
#include "renderer/gfx/objects/Shader.h"
#include "core/Camera.h"

#include "renderer/texture/Texture.h"

namespace Jukcraft {
	class LivingEntityRenderer {
	public:
		LivingEntityRenderer();
		virtual ~LivingEntityRenderer();
		void render(LivingEntity& livingEntity, float partialTicks);
	private:
		gfx::VertexArray vao;
		gfx::Buffer vbo;
		gfx::Shader shader;
		Texture texture;

		EntityModel model;
		Bone::Quad* mappedPointer;
	};
}
