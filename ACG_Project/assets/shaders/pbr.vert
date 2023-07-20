#version 420 core

///////////////////////////////////////////////////////////////////////////////
// Input vertex attributes
///////////////////////////////////////////////////////////////////////////////
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoordIn;


///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


///////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
///////////////////////////////////////////////////////////////////////////////
out vec2 texCoord;
out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
out mat4 viewInverse;

void main()
{
	mat4 modelViewMatrix = viewMatrix*modelMatrix;
	mat4 normalMatrix = inverse(transpose(modelViewMatrix));
	
	gl_Position = projMatrix*viewMatrix*modelMatrix * vec4(position, 1);
	viewSpaceNormal = (normalMatrix * vec4(normal, 0.0)).xyz;
	viewSpacePosition = (modelViewMatrix * vec4(position, 1)).xyz;
	texCoord = texCoordIn;
	viewInverse = inverse(viewMatrix);
}
