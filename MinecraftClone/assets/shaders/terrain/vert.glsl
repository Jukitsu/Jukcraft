#version 460 core

layout(location = 0) in uint a_VertexData;
layout(location = 1) in uint a_VertexLight;

layout(location = 0) out VS_OUT {
	vec3 v_TexCoords;
	float v_Shading;
	float v_Light;
} vs_Out;

const vec2 c_TexCoords[4] = vec2[4] (
	vec2(0.0f, 1.0f),
	vec2(0.0f, 0.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f)
);

layout(std140, binding = 0) uniform u_Camera {
	mat4 u_CameraTransforms;
	vec4 u_CameraPos;
};

layout(location = 0) uniform vec3 u_ChunkPos;

void main(void) {
	vec3 pos = u_ChunkPos + vec3(a_VertexData >> 17 & 0x1F, a_VertexData >> 22, a_VertexData >> 12 & 0x1F);

	gl_Position = u_CameraTransforms * vec4(pos, 1.0f);

	vs_Out.v_TexCoords = vec3(c_TexCoords[a_VertexData >> 10 & 0x3], a_VertexData >> 2 & 0xFF);
	vs_Out.v_Shading = float((a_VertexData & 0x3) + 2) / 5.0f;
	vs_Out.v_Light = pow(0.8f, 15 - a_VertexLight);
}