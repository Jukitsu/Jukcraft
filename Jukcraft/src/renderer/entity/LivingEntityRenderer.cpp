#include "pch.h"
#include "renderer/entity/LivingEntityRenderer.h"

namespace Jukcraft {
	LivingEntityRenderer::LivingEntityRenderer() 
		:shader("assets/shaders/entity/vert.glsl", "assets/shaders/entity/frag.glsl"),
		model("assets/models/curry.json"), texture("assets/textures/curry.png") {
		vbo.allocate(60 * sizeof(Bone::Quad), nullptr, true);
		mappedPointer = reinterpret_cast<Bone::Quad*>(vbo.map(0, 60 * sizeof(Bone::Quad)));

		vao.bindLayout(
			gfx::VertexArrayLayout{ 
				{
					{ 3, true },
					{ 2, true },
					{ 3, true }
				}		
			}
		);

		vao.bindVertexBuffer(
			vbo, 0,
			gfx::VertexBufferLayout{
				{
					{ 0, 0 }, 
					{ 1, offsetof(Bone::Vertex, texUV)}, 
					{ 2, offsetof(Bone::Vertex, normal)}
				},
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
		bool isHurt = livingEntity.isHurt() && livingEntity.deathTime <= TICK_RATE;

		glm::vec4 color = glm::vec4(1.0f);

		if (isHurt)
			shader.setUniform4f(0, glm::vec4(1.0f, 0.0f, 0.0f, 0.6f));
		else
			shader.setUniform4f(0, glm::vec4(1.0f));

		

		for (auto&& [name, bone] : model.bones) {
			glm::mat4 pose(1.0f);
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), interpolatedPos);

			pose = glm::rotate(pose, -interpolatedBodyRot.x - glm::pi<float>() / 2, UP);
			pose = glm::rotate(pose, interpolatedBodyRot.y, EAST);

			if (livingEntity.getDeathTime() > 0.0f) {
				float i = glm::min(1.0f,
					glm::sqrt(
						(float)(livingEntity.getDeathTime() + partialTicks - 1.0F) / TICK_RATE * 1.6F));
				pose = glm::rotate(pose, i * glm::pi<float>() / 2.0f, SOUTH);
			}
			
			if (bone.isHead) {
				pose = glm::translate(pose, bone.pivot);
				pose = glm::rotate(pose, interpolatedBodyRot.y, WEST);
				pose = glm::rotate(pose, -interpolatedBodyRot.x - glm::pi<float>() / 2, DOWN);
				
				
				pose = glm::rotate(pose, -interpolatedHeadRot.x - glm::pi<float>() / 2, UP);
				pose = glm::rotate(pose, interpolatedHeadRot.y, EAST);
				pose = glm::translate(pose, -bone.pivot);
			}

			const float phase = glm::pi<float>() * (bone.isArm ^ bone.isOdd);

			if (bone.isLeg) {
				pose = glm::translate(pose, bone.pivot);
				
				pose = glm::rotate(
					pose,
					glm::cos(
						livingEntity.getWalkAnimation().lerpPos(partialTicks)
						* 0.6662f + phase
					) * 0.07f * TICK_RATE * glm::min(20.0f / TICK_RATE, 
						livingEntity.getWalkAnimation().lerpSpeed(partialTicks)
					),
					EAST
				);
				pose = glm::translate(pose, -bone.pivot);
			}

			if (bone.isArm) {
				pose = glm::translate(pose, bone.pivot);
				float theta = bone.isOdd ? (float)livingEntity.getAge() / (-TICK_RATE) : 2 * livingEntity.getAge() / TICK_RATE;
				pose = glm::rotate(pose, glm::sin(theta + phase) / 8, UP);
				pose = glm::rotate(
					pose,
					glm::cos(
						livingEntity.getWalkAnimation().lerpPos(partialTicks)
						* 0.6662f + phase
					) * 0.05f * TICK_RATE * glm::min(20 / TICK_RATE, 
						livingEntity.getWalkAnimation().lerpSpeed(partialTicks)
					),
					EAST
				);
				pose = glm::rotate(pose, glm::cos(theta + phase) / 8, EAST);
				pose = glm::translate(pose, -bone.pivot);
			}

						

			std::vector<Bone::Quad> quads = bone.quads;

			for (auto& quad : quads) {
				for (auto& vertex : quad.vertices) {
					vertex.pos = modelMatrix * pose * glm::vec4(vertex.pos, 1.0f);
				}
				glm::vec3 normal = glm::normalize(glm::cross(
					quad.vertices[0].pos - quad.vertices[1].pos,
					quad.vertices[0].pos - quad.vertices[2].pos
				));
				for (auto& vertex : quad.vertices) {
					vertex.normal = normal;
				}
			}


			vbo.sync();
			memcpy(mappedPointer, quads.data(), quads.size() * sizeof(Bone::Quad));
			vbo.flush(0, quads.size() * sizeof(Bone::Quad));
			shader.bind();
			
			Renderer::DrawElements(vao, quads.size() * 6);
			vbo.addFence();
			i++;
		}
		
	}
}