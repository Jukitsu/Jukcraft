#include "pch.h"
#include "renderer/entity/LivingEntityRenderer.h"

namespace Jukcraft {
	LivingEntityRenderer::LivingEntityRenderer() 
		:shader("assets/shaders/entity/vert.glsl", "assets/shaders/entity/frag.glsl"),
		model("assets/models/zombie.json"), texture("assets/textures/zombie.png") {
		vbo.allocate(sizeof(Bone::quads), nullptr, true);
		mappedPointer = reinterpret_cast<Bone::Quad*>(vbo.map(0, sizeof(Bone::quads)));

		vao.bindLayout(
			gfx::VertexArrayLayout{ 
				{
					{ 3, true },
					{ 2, true }
				}		
			}
		);

		vao.bindVertexBuffer(
			vbo, 0,
			gfx::VertexBufferLayout{
				{{ 0, 0 }, { 1, offsetof(Bone::Vertex, texUV)}},
				0,
				sizeof(Bone::Vertex)
			}
		);

		vao.bindIndexBuffer(Renderer::GetChunkIbo());

		texture.setSamplerUnit(1);
	}

	LivingEntityRenderer::~LivingEntityRenderer() {

	}

	void LivingEntityRenderer::render(LivingEntity& livingEntity, float partialTicks) {
		glm::vec3 interpolatedPos = glm::mix(livingEntity.getOld().position, livingEntity.getPos(), partialTicks);
		glm::vec2 interpolatedBodyRot = glm::mix(livingEntity.getOld().bodyRot, livingEntity.getBodyRot(), partialTicks);
		glm::vec2 interpolatedHeadRot = glm::mix(livingEntity.getOld().headRot, livingEntity.getHeadRot(), partialTicks);

		int i = 0; 
		for (auto&& [name, bone] : model.bones) {
			glm::mat4 pose(1.0f);
			pose = glm::translate(pose, interpolatedPos);
			
			if (name == "head") {
				pose = glm::translate(pose, bone.pivot);
				pose = glm::rotate(pose, -interpolatedHeadRot.x - glm::pi<float>() / 2, glm::vec3(0.0f, 1.0f, 0.0f));
				pose = glm::rotate(pose, interpolatedHeadRot.y, glm::vec3(1.0f, 0.0f, 0.0f));
				pose = glm::translate(pose, -bone.pivot);
			}
			else {
				pose = glm::rotate(pose, -interpolatedBodyRot.x - glm::pi<float>() / 2, glm::vec3(0.0f, 1.0f, 0.0f));
				pose = glm::rotate(pose, interpolatedBodyRot.y, glm::vec3(1.0f, 0.0f, 0.0f));
			}
			
			

			std::array<Bone::Quad, 6> quads = bone.quads;

			for (auto& quad : quads) {
				for (auto& vertex : quad.vertices) {
					vertex.pos = pose * glm::vec4(vertex.pos, 1.0f);
				}
			}

			vbo.sync();
			memcpy(mappedPointer, quads.data(), sizeof(quads));
			vbo.flush(0, sizeof(quads));
			shader.bind();
			
			Renderer::DrawElements(vao, 6 * 6);
			vbo.addFence();
			i++;
		}
		
	}
}