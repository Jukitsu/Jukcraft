#pragma once
#include "physics/Collider.h"

namespace Jukcraft {
	static const std::vector<Quad> cube = {
		Quad {
			Vertex { { 1, 1, 1 },		0,	0,	1 },
			Vertex { { 1, 0, 1 },		1,	0,	1 },
			Vertex { { 1, 0, 0 },		2,	0,	1 },
			Vertex { { 1, 1, 0 },		3,	0,	1 },
		},

		Quad {
			Vertex { { 0, 1, 0 },		0,	0,	1 },
			Vertex { { 0, 0, 0 },		1,	0,	1 },
			Vertex { { 0, 0, 1 },		2,	0,	1 },
			Vertex { { 0, 1, 1 },		3,	0,	1 }
		},

		Quad {
			Vertex { { 1, 1, 1 },		0,	0,	3 },
			Vertex { { 1, 1, 0 },		1,	0,	3 },
			Vertex { { 0, 1, 0 },		2,	0,	3 },
			Vertex { { 0, 1, 1 },		3,	0,	3 }
		},

		Quad {
			Vertex { { 0, 0, 1 },		0,	0,	0 },
			Vertex { { 0, 0, 0 },		1,	0,	0 },
			Vertex { { 1, 0, 0 },		2,	0,	0 },
			Vertex { { 1, 0, 1 },		3,	0,	0 }
		},

		Quad {
			Vertex { { 0, 1, 1 },		0,	0,	2 },
			Vertex { { 0, 0, 1 },		1,	0,	2 },
			Vertex { { 1, 0, 1 },		2,	0,	2 },
			Vertex { { 1, 1, 1 },		3,	0,	2 }
		},

		Quad {
			Vertex { { 1, 1, 0 },		0,	0,	2 },
			Vertex { { 1, 0, 0 },		1,	0,	2 },
			Vertex { { 0, 0, 0 },		2,	0,	2 },
			Vertex { { 0, 1, 0 },		3,	0,	2 }
		}
	};

	static const std::vector<Quad> air = {};

	class Model {
	public:
		virtual const std::vector<Quad>& getQuads() const = 0;
		constexpr const std::vector<Collider>& getColliders() const {
			return colliders;
		};
	protected:
		std::vector<Collider> colliders;
	};

	class Cube : public Model {
	public:
		Cube() {
			colliders.push_back(
				{
					glm::vec3(0.0f),
					glm::vec3(1.0f)
				}
			);
		}
		const std::vector<Quad>& getQuads() const override {
			return cube;
		}
		
	};

	class Air : public Model {
	public:

		const std::vector<Quad>& getQuads() const override {
			return air;
		}
	};

	inline const struct Models {
		Air air;
		Cube cube;
	} models;

}
