#version 420 core

layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoordIn;

uniform mat4 u_viewproj;
uniform mat4 u_transform;

out vec2 texCoord; // outgoing interpolated texcoord to fragshader
out vec3 ws_normal;

void main()
{
	gl_Position = u_viewproj*u_transform* vec4(position, 1.0);
	texCoord = texCoordIn;
	ws_normal = vec3(u_transform * vec4(normal, 0));
}