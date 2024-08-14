#version 460 core

layout(early_fragment_tests) in;

layout(location = 0) in VS_OUT {
	vec2 v_TexCoords;
	float v_Shading;
} fs_In;


layout(location = 1) uniform vec4 u_Overlay;
layout(binding = 1) uniform sampler2D u_TextureSampler;

layout(location = 0) out vec4 fragColor;

void main(void) {
	vec4 color = texture(u_TextureSampler, fs_In.v_TexCoords);
	color.rgb = mix(u_Overlay.rgb, color.rgb, u_Overlay.a);
	color.rgb *= fs_In.v_Shading;
	fragColor = color;
}