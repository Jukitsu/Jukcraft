#version 460 core

layout(location = 0) in uint a_VertexData;
layout(location = 1) in uint a_VertexLight;

layout(location = 0) out VS_OUT {
	vec3 v_TexCoords;
	vec3 v_Light;
	float v_Shading;
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
layout(location = 1) uniform float u_Daylight;

void main(void) {
	vec3 pos = u_ChunkPos + vec3(a_VertexData >> 17 & 0x1F, a_VertexData >> 22, a_VertexData >> 12 & 0x1F);

	uint blockLight = a_VertexLight & 0xF;
	uint skyLight = (a_VertexLight >> 4) & 0xF;

	float blocklightMultiplier = pow(0.8, 15.0 - blockLight);
	float intermediateSkylightMultiplier = pow(0.8, 15.0 - skyLight * u_Daylight);
	float skylightMultiplier = pow(0.8, 15.0 - skyLight);

	gl_Position = u_CameraTransforms * vec4(pos, 1.0f);

	vs_Out.v_TexCoords = vec3(c_TexCoords[a_VertexData >> 10 & 0x3], a_VertexData >> 2 & 0xFF);
	vs_Out.v_Light = vec3(
		clamp(blocklightMultiplier * 1.5, intermediateSkylightMultiplier, 1.0), 
		clamp(blocklightMultiplier * 1.25, intermediateSkylightMultiplier, 1.0), 
		clamp(skylightMultiplier, blocklightMultiplier, 1.0)
	);
	vs_Out.v_Shading = pow(float((a_VertexData & 0x3) + 2) / 5.0f, 2.2f);
}