#version 460 core

layout(location = 0) in uint a_VertexData;
layout(location = 1) in uint a_SmoothLightData;

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

const float c_Gamma = 1.0f;


layout(std140, binding = 0) uniform u_Camera {
	mat4 u_CameraTransforms;
	vec4 u_CameraPos;
};

layout(location = 0) uniform vec3 u_ChunkPos;
layout(location = 1) uniform float u_Daylight;

void main(void) {
	vec3 pos = u_ChunkPos + vec3(a_VertexData >> 17 & 0x1F, a_VertexData >> 22, a_VertexData >> 12 & 0x1F);

	float ao = 0.2f + float(a_SmoothLightData & 0x7) * 0.2f;
	float blockLight = float((a_SmoothLightData >> 3) & 0x3F) / 4.0f;
	float skyLight = float(a_SmoothLightData >> 9) / 4.0f;
	float torchLight = blockLight / 15.0f;

	float intermediateSkylightMultiplier = pow(0.8, 15.0 - skyLight * u_Daylight);
	float skylightMultiplier = pow(0.8, 15.0 - skyLight);

	
	float blocklightRedTint = pow(0.8, 15.0 - blockLight);
	float blocklightGreenTint = pow(0.8, 15.0 - blockLight);
	float blocklightBlueTint = pow(0.8, 15.0 - blockLight * blockLight / 15.0f);
	float skyBlueTint = pow(0.8, 15.0 - skyLight * u_Daylight * (2.0 - u_Daylight));

	float shading = float(2 + (a_VertexData & 0x3)) / 5.0f;

	gl_Position = u_CameraTransforms * vec4(pos, 1.0f);

	vs_Out.v_TexCoords = vec3(c_TexCoords[a_VertexData >> 10 & 0x3], a_VertexData >> 2 & 0xFF);
	vs_Out.v_Light = pow(
		vec3(
			clamp(blocklightRedTint, intermediateSkylightMultiplier, 1.0), 
			clamp(blocklightGreenTint, intermediateSkylightMultiplier, 1.0), 
			clamp(skyBlueTint, blocklightBlueTint, 1.0)
		),
		vec3(1.0 - c_Gamma * 0.5)
	);
	vs_Out.v_Shading = shading * ao;
}