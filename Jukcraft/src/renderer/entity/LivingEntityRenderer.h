#pragma once
#include "models/entity/EntityModel.h"
#include "entity/LivingEntity.h"
#include "renderer/Renderer.h"
#include "core/Camera.h"
#include "renderer/gfx/buffers/DynamicBuffer.h"
#include "renderer/gfx/objects/Shader.h"
#include "renderer/gfx/objects/VertexArray.h"

#include "renderer/texture/Texture.h"

namespace Jukcraft {
	class LivingEntityRenderer {
	public:
		LivingEntityRenderer();
		virtual ~LivingEntityRenderer();
		void beginRenderPass();
		void compile(LivingEntity& livingEntity, float partialTicks);
		void endRenderPass();
	private:
		gfx::VertexArray vao;
		gfx::DynamicBuffer<Bone::Quad> vbo;
		gfx::Shader shader;

		Texture texture;

		size_t currentQuadCount = 0;

		EntityModel model;
		Bone::Quad* mappedPointer;
	};
}
