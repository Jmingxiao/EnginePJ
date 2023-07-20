#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;

layout(binding = 0) uniform sampler2D environmentMap;

in vec2 texCoord;

uniform uint num_samples;

uniform uint samples_taken;

uniform float a_roughness;


const float M_PI = 3.1415926538;

float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * M_PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  

vec2 worldtosph(vec3 dir){

	float theta = acos(max(-1.0f, min(1.0f, dir.z)));
	float phi = atan(dir.y, dir.x);
	if(phi < 0.0f)
		phi = phi + 2.0f * M_PI;
	// Use these to lookup the color in the environment map
	return vec2(phi / (2.0 * M_PI), theta / M_PI);
}

void main()
{
	float roughness = a_roughness/7.0f;	
	float phi = (texCoord.x)*2*M_PI;
	float theta = texCoord.y * M_PI;
	vec3 n = normalize(vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)));

	vec3 R = n;
    vec3 V = R;

	const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     

	for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, n, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(n, L), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(environmentMap, worldtosph(L)).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

	prefilteredColor = prefilteredColor / totalWeight;
    fragmentColor.xyz = prefilteredColor;
    fragmentColor.a =1;
}
