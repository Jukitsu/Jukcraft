#version 460 core

layout(early_fragment_tests) in;

layout(location = 0) in VS_OUT {
	vec2 v_TexCoords;
} fs_In;

layout(binding = 1) uniform sampler2D u_TextureSampler;

layout(location = 0) out vec4 fragColor;

void main(void) {
	vec4 texel = texture(u_TextureSampler, fs_In.v_TexCoords);
	fragColor = texel;
}