#version 460 core

// layout(early_fragment_tests) in;

layout(location = 0) in VS_OUT {
	vec3 v_TexCoords;
	vec3 v_Light;
	float v_Shading;
} fs_In;

layout(binding = 0) uniform sampler2DArray u_TextureArraySampler;

layout(location = 0) out vec4 fragColor;

void main(void) {
	vec4 texel = texture(u_TextureArraySampler, fs_In.v_TexCoords);
	fragColor = texel * vec4(fs_In.v_Light * fs_In.v_Shading, 1.0f);
}