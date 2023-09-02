#version 460 core

layout(early_fragment_tests) in;

layout(location = 0) in VS_OUT {
	vec3 v_TexCoords;
	float v_Shading;
	float v_Light;
} fs_In;

layout(binding = 0) uniform sampler2DArray u_TextureArraySampler;

layout(location = 0) out vec4 fragColor;

void main(void) {
	fragColor = texture(u_TextureArraySampler, fs_In.v_TexCoords) * fs_In.v_Shading * fs_In.v_Light;
}