#version 420 core
precision highp float;

layout(binding = 0) uniform sampler2D colorMap;

uniform vec3 material_color;
in vec2 texCoord;
in vec3 normal;

layout(location = 0) out vec4 fragmentColor;

void main()
{
    
    vec4 color;

    color =texture(colorMap, texCoord);
  	fragmentColor = vec4(color.rgb,1.0f);
}