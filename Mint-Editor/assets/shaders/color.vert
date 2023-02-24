#version 420 core

layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoordIn;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 texCoord; // outgoing interpolated texcoord to fragshader
out vec3 ws_normal;

void main()
{
	gl_Position = projMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);
	texCoord = texCoordIn;
	ws_normal = vec3(modelMatrix * vec4(normal, 0));
}