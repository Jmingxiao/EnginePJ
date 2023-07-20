#version 420

layout(binding = 0) uniform sampler2D normaltex;
layout(binding = 1) uniform sampler2D positiontex;
layout(binding = 2) uniform sampler2D noiseTex;

layout(location = 0) out vec4 fragmentColor;
uniform mat4 inverseProjectionMatrix;
uniform mat4 projMatrix;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

const vec2 noiseScale =vec2(800.0/4.0, 600.0/4.0); 


vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}


void main()
{
	vec2 texCoord = gl_FragCoord.xy;
	vec3 posvs = textureRect(positiontex, texCoord).rgb;
	vec3 normalvs = normalize(textureRect(normaltex, texCoord).rgb );
	vec3 randomVec = normalize(texture(noiseTex, texCoord/ textureSize(noiseTex, 0) * noiseScale).xyz);

    vec3 tangentvs = normalize(randomVec - normalvs * dot(randomVec, normalvs));
    vec3 bitangentvs = cross(normalvs, tangentvs);
    mat3 tbn = mat3(tangentvs, bitangentvs, normalvs);

    
	float occlusion = 0.0;
	for (int i = 0 ; i < kernelSize ; i++) {

		vec3 samplePos =  tbn*samples[i];// generate a random point
        samplePos = posvs +samplePos*radius;


		vec4 offset = vec4(samplePos, 1.0); // make it a 4-vector
        offset = projMatrix * offset; // project on the near clipping plane
        offset.xy /= offset.w; // perform perspective divide
        offset.xy = offset.xy * 0.5 + 0.5; // transform to (0,1) range

        float sampleDepth = texture(positiontex, offset.xy).z;

       float rangeCheck = smoothstep(0.0, 1.0, radius / abs(posvs.z - sampleDepth));
       occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;    
    }
	 occlusion = 1.0 - (occlusion / kernelSize);

    fragmentColor = vec4(occlusion);
}