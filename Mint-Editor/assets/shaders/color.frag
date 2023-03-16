#version 420 core
precision highp float;

uniform vec3 material_color = vec3(1.0);
uniform vec3 material_emission = vec3(0.0);
uniform int entity_id ;

uniform int has_color_texture = 0;
layout(binding = 0) uniform sampler2D color_texture;

uniform int has_emission_texture = 0;
layout(binding = 5) uniform sampler2D emission_texture;

in vec2 texCoord;
in vec3 ws_normal;

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out int entid;


void main()
{
	vec3 n = normalize(ws_normal);
	float ambientfactor =0.2f;
	vec4 color = vec4(material_color, 1.0);
	if(has_color_texture == 1)
	{
		color = texture(color_texture, texCoord.xy);
	}

	vec4 emission = vec4(material_emission, 0);
	if(has_emission_texture == 1)
	{
		emission = texture(emission_texture, texCoord.xy);
	}
	const vec3 lightcolor = vec3(1.0);
	const vec3 lightDir = normalize(vec3(-0.74, -1, 0.68));

	fragmentColor.rgb = color.rgb* (max(dot(n, -lightDir), 0.0001)+ambientfactor)*lightcolor + emission.rgb;
	fragmentColor.a = color.a;
	entid = entity_id;
}