#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out int fragmentColor;
layout(binding = 0) uniform isampler2DMS image;

in vec2 texCoord;


void main()
{
	int entity_id =texelFetch(image,ivec2(gl_FragCoord.xy),0).r;
	int id =entity_id>(1024*1024)||entity_id<0? -1: entity_id;
	fragmentColor = id;
}