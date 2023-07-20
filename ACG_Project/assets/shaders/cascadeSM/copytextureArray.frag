#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;

uniform sampler2DArray depthMap;

uniform int layer;

in vec2 texCoord;

void main()
{             
    float depthValue = texture(depthMap, vec3(texCoord, layer)).r;
    fragmentColor = vec4(vec3(depthValue), 1.0); // orthographic
}