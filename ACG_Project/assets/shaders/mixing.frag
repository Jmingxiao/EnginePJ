#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;

layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) uniform sampler2D normaltex;
layout(binding = 2) uniform sampler2D positiontex;
layout(binding = 3) uniform sampler2D ssrTex;
layout(binding = 4) uniform sampler2D baselightTex;


//shadow uniform input 
layout(binding = 10) uniform sampler2DArray shadowMapTex;
layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[8];
};
uniform float farPlane;
uniform float cascadePlaneDistances[16];
uniform int cascadeCount;


//Other uniform Input
uniform vec3 lightDirection;
uniform mat4 viewInverse;
uniform bool ssr_on;

in vec2 texCoord;

float calculateShadow(vec4 posvs,vec3 viewSpaceNormal)
{
	float visibility =1.0f;
	float depth = abs(posvs.z);
    vec3 worldSpacePos = (viewInverse* vec4(posvs.xyz,1)).xyz;

	int layer = -1;
    int blendingmax = 3; 
    float blending =0.0f;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depth < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

	vec4 posLS = lightSpaceMatrices[layer] * vec4(worldSpacePos, 1.0);
	vec3 shadowCoords = posLS.xyz / posLS.w;
	shadowCoords = shadowCoords * 0.5 + 0.5;
	float currentDepth = shadowCoords.z;
	if(currentDepth>1.0){
		return visibility;
	}
	vec3 normalvs = normalize(viewSpaceNormal);
	vec3 lightDir = normalize(lightDirection);
    float bias = clamp(0.05 * (1.0 - dot(normalvs, lightDir)), 0.001,0.05);
	if (layer == cascadeCount)
	{
		bias *= 1 / (farPlane*0.1);
	}
	else
	{
		bias *= 1 / (cascadePlaneDistances[layer]*0.1);
	}
    
	// PCF
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMapTex, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapTex, vec3(shadowCoords.xy + 2*vec2(x, y) * texelSize, layer)).r;
            visibility += (currentDepth - bias) > pcfDepth ? 0.0 : 1.0;        
        }    
    }
    visibility /= 9.0;


	return visibility;
}

void main()
{
    vec3 color =texture(colorTex, texCoord).rgb;
    vec3 normalvs = texture(normaltex,texCoord).rgb;
    vec4 posvs = texture(positiontex,texCoord);
    float roughness = texture(normaltex,texCoord).a;
    float fresnel = texture(colorTex, texCoord).a;
    vec4 ssrcolor = texture(ssrTex,texCoord);
    vec4 baselightfactor = texture(baselightTex,texCoord);

    vec3 fragcolor = ssrcolor.a<0.5||!ssr_on?color:baselightfactor.rgb+ssrcolor.rgb;

    float visibility=0.0f;
	{
		visibility = calculateShadow(posvs,normalvs);
	}
    fragcolor*=visibility;
	fragmentColor = vec4(fragcolor,1.0);
}