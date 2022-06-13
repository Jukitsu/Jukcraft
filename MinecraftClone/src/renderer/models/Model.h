#pragma once
static const std::array<Quad, 6> cube = {
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

