#version 460 core

layout(location = 0) in vec3 a_VertexPos;
layout(location = 1) in vec2 a_TexUV;

layout(location = 0) out VS_OUT {
	vec2 v_TexCoords;
} vs_Out;

layout(std140, binding = 0) uniform u_Camera {
	mat4 u_CameraTransforms;
	vec4 u_CameraPos;
};


void main(void) {
	gl_Position = u_CameraTransforms * vec4(a_VertexPos, 1.0f);
	vs_Out.v_TexCoords = a_TexUV;
}