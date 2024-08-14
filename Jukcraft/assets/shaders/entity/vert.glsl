#version 460 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexUV;
layout(location = 2) in vec3 a_Normal;

layout(location = 0) out VS_OUT {
	vec2 v_TexCoords;
	float v_Shading;
} vs_Out;

layout(location = 0) uniform float u_Light;
layout(std140, binding = 0) uniform u_Camera {
	mat4 u_CameraTransforms;
	vec4 u_CameraPos;
};

const vec3 c_Sunlight = normalize(vec3(0.0, 2.0, 1.0));

void main(void) {
	gl_Position = u_CameraTransforms * vec4(a_Pos, 1.0f);
	vs_Out.v_TexCoords = a_TexUV;
	vs_Out.v_Shading = 
		max(0.4f, (1. + dot(abs(a_Normal.xz), c_Sunlight.xz) + a_Normal.y * c_Sunlight.y) / 2.0f)
		* u_Light; 
}