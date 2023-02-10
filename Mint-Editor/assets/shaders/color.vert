#version 420 core

layout (location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texturecoord;

uniform mat4 u_viewproj;
uniform mat4 u_transform;

out vec2 texCoord;
out vec3 normal;

void main(){

	gl_Position =  u_viewproj*u_transform*vec4(a_position, 1.0);
	texCoord  = a_texturecoord;
	normal = a_normal;
}