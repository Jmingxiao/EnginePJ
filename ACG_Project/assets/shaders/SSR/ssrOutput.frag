#version 420

precision highp float;

#define PI 3.14159265359

layout(binding = 0) uniform sampler2D colortex;
layout(binding = 1) uniform sampler2D normaltex;
layout(binding = 2) uniform sampler2D positiontex;
layout(binding = 3) uniform sampler2D baseColortex;

//input matrixs
uniform mat4 viewInverse;
uniform mat4 inverseProjectionMatrix;
uniform mat4 projMatrix;
uniform mat4 projviewMatrix;
uniform float zfar;
uniform float znear;

in vec2 texCoord;


layout(location = 0) out vec4 fragmentColor;


vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}



float gauss[] = float[]
(
    0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067,
    0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
    0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
    0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771,
    0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
    0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
    0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067
);	

vec3 GetGaussColor(vec2 uv,vec2 sceensize)
{
    const int size = 7;
 
    vec3 finalColor = vec3(0,0,0);
 
    int idx = 0;
    for(int i = -3;i <= 3;i++)
    {
        for(int j = -3; j <= 3;j++)
        {
            vec2 offset_uv = uv + vec2(5.0 * i /sceensize.x, 5.0 * j /sceensize.y);
            vec3 color = texture(colortex, offset_uv).xyz;
            float weight = gauss[idx++];
            finalColor = finalColor + weight * color;
        }
    }
 
    return finalColor;
}

//

float maxDist =15.0;
float path = 0.2f;
float thickness =0.5f;
vec2 texSize;

vec2 computesspos(vec3 vs)
{
	vec4 projpos = projMatrix*vec4(vs,1);
	vec2 screenpos = projpos.xy/projpos.w;
	screenpos = screenpos*0.5+0.5;
	return screenpos;
}

vec4 computefragpos(vec3 vs,vec2 texSize)
{
	vec4 projpos = projMatrix*vec4(vs,1);
	projpos.xyz = projpos.xyz/projpos.w;
	projpos.xy = projpos.xy*0.5+0.5;
	projpos.xy*=texSize;
	return projpos;
}

vec3 computeindirectSpec(vec3 color,float F, float metalness, vec3 basecolor)
{
	return color*F*(1-metalness)+ F*basecolor*color*metalness;
}

uniform float cb_fadeStart = 0.8f;
uniform float cb_fadeEnd  = 1.0f;


void main()
{
	texSize = textureSize(positiontex, 0);
	vec4 posvs = texture(positiontex, texCoord);
	vec3 normalvs = normalize(texture(normaltex, texCoord).rgb );
	float F = texture(colortex, texCoord).a;
	float roughness =  texture(normaltex, texCoord).a;
	vec3 color = texture(colortex,texCoord).rgb;
	vec3 basecolor = texture(baseColortex,texCoord).rgb;
	float metalness =  texture(baseColortex,texCoord).a;

	//apply jittering
	mat4 dither = mat4(
	   0,       0.5,    0.125,  0.625,
	   0.75,    0.25,   0.875,  0.375,
	   0.1875,  0.6875, 0.0625, 0.5625,
	   0.9375,  0.4375, 0.8125, 0.3125
	);
	int sampleCoordX = int(mod((texSize.x * texCoord.x),4));
	int sampleCoordY = int(mod((texSize.y * texCoord.y),4));
	float offset = dither[sampleCoordX][sampleCoordY];

	if (posvs.w <= 0.0|| F <= 0.0) { fragmentColor = vec4(0.0); return; }

	vec3 reflectVec = normalize(reflect(posvs.xyz, normalvs));
	vec3 envRefl =vec3(0);

	vec3 reflecolor = vec3(0);

	vec3 currentpos = posvs.xyz;
	
	float march = maxDist/path;
	vec2 screenpos= computesspos(posvs.xyz);
	vec2 uv = screenpos;
	currentpos += normalize(texture(normaltex,screenpos).rgb)*0.05f;
	vec3 raypos =posvs.xyz;
	fragmentColor.a =0.0f;
	int i;
	for(i=0; i<march;i++){
		vec3 prevpos = currentpos;
		currentpos += (offset*0.1f+path)*reflectVec;
		screenpos = computesspos(currentpos);
		if(screenpos.x>=1||screenpos.y>=1||screenpos.x<=0||screenpos.y<=0){break;}
		float sampledepthb = abs(texture(positiontex,screenpos).z);
		float d = abs(currentpos.z);
		float depthb = d -sampledepthb;
		if(depthb>0){
			if(depthb<thickness){
				raypos = currentpos;
				uv = screenpos;
				reflecolor = texture(colortex,screenpos).rgb;
				fragmentColor.a =1.0f;
				break;
			}
			int count = 0;
			vec3 begin = prevpos;
			vec3 end = currentpos;
			vec3 currPos = (begin + end)/2;
			// binary search
			while(true)
			{
			    count++;
				screenpos = computesspos(currPos);
			    float sampledepthb = abs(texture(positiontex,screenpos).z);
				float d = abs(currPos.z);
				float depthb =  d -sampledepthb;
			    if(abs(depthb) < 0.3 *thickness)
			    {	
					raypos = currPos;
					uv = screenpos;
					reflecolor = texture(colortex,screenpos).rgb;
					fragmentColor.a =1.0f;
			        break;
			    }
			    if(depthb>0)
			    {
			        end = currPos;
			    }
			    else if(depthb<0)
			    {
			        begin = currPos;
			    }                          
			    currPos = (begin + end)/2;
			    if(count >3)
			    {	
			        break;
				}
			}
		}
	}

	vec2 boundary  = abs(uv.xy - vec2(0.5f, 0.5f)) * 2.0f;
	const float fadeDiffRcp = 1.0f / (cb_fadeEnd - cb_fadeStart);
	float fadeOnBorder = 1.0f - clamp((boundary.x - cb_fadeStart) * fadeDiffRcp,0.0,1.0);
	fadeOnBorder *= 1.0f - clamp((boundary.y - cb_fadeStart) * fadeDiffRcp,0.0,1.0);
	fadeOnBorder = smoothstep(0.0f, 1.0f, fadeOnBorder);
	float fadeOnDistance = 1.0f - clamp(distance(raypos, posvs.xyz)/(maxDist*2.0),0.0,1.0);
	float glossy = 1- roughness;
	float glossyfade=clamp(mix(0.0f,1.0f,glossy*5.0f),0.0f,1.0f);
	reflecolor*=fadeOnBorder*fadeOnDistance;// *glossyfade;

	reflecolor = computeindirectSpec(reflecolor,F,metalness,basecolor);
    fragmentColor.rgb = vec3(reflecolor);
	
}





// UE4 Random.ush
//// 3D random number generator inspired by PCGs (permuted congruential generator).
//uvec3 Rand3DPCG16(uvec3 p)
//{
//	uvec3 v = uvec3(p);
//	v = v * 1664525u + 1013904223u;
//
//	// That gives a simple mad per round.
//	v.x += v.y*v.z;
//	v.y += v.z*v.x;
//	v.z += v.x*v.y;
//	v.x += v.y*v.z;
//	v.y += v.z*v.x;
//	v.z += v.x*v.y;
//
//	// only top 16 bits are well shuffled
//	return v >> 16u;
//}
//vec2 UniformSampleDisk(vec2 E)
//{
//	float Theta = 2 * PI * E.x;
//	float Radius = sqrt(E.y);
//	return Radius * vec2(cos(Theta), sin(Theta));
//}
//
//float RadicalInverse_VdC(uint bits) 
//{
//     bits = (bits << 16u) | (bits >> 16u);
//     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
//     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
//     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
//     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
//     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
//}
//// ----------------------------------------------------------------------------
//vec2 Hammersley(uint i, uint N)
//{
//	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
//}
//
//vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
//{
//    float a = roughness*roughness;
//	
//    float phi = 2.0 * PI * Xi.x;
//    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
//    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
//	
//    // from spherical coordinates to cartesian coordinates
//    vec3 H;
//    H.x = cos(phi) * sinTheta;
//    H.y = sin(phi) * sinTheta;
//    H.z = cosTheta;
//	
//    // from tangent-space vector to world-space sample vector
//    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
//    vec3 tangent   = normalize(cross(up, N));
//    vec3 bitangent = cross(N, tangent);
//	
//    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
//    return normalize(sampleVec);
//}  
//



//glossy 
/*vec3 normalws = (viewInverse* vec4(normalvs,0.0f)).xyz;
uvec2 fragpos  = uvec2(texCoord*texSize);
uvec2 random = Rand3DPCG16(ivec3(fragpos, frameindex)).xy;				
vec2 E = Hammersley16(i, NumRays, Random);*/


//dda

// vec3 endvs = posvs.xyz+reflectVec*(maxDist);
//	vec4 startfrag = computefragpos(posvs.xyz,texSize);
//	vec4 endfrag = computefragpos(endvs,texSize);
//	vec2 frag  = startfrag.xy;
//	vec4 uv = vec4(0.0);
//	uv.xy = frag / texSize;
//	float deltaX    = endfrag.x - startfrag.x;
//	float deltaY    = endfrag.y - startfrag.y;
//
//	float search0 = 0;
//	float search1 = 0;
//
//	int hit0 = 0;
//	int hit1 = 0;
//	
//	float viewDistance = posvs.y;
//	float depth  = thickness;
//	vec4 curpos = posvs;
//
//
//	float useX      = abs(deltaX) >= abs(deltaY) ? 1 : 0;
//	float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(0.3, 0, 1);//resolution=0.5
//	vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);
//
//	int steps =10;
//	float i;
//	for ( i = 0; i < 1000.0f; i+=1.0f) {
//		frag += increment;
//		uv.xy      = frag / texSize;
//		curpos = texture(positiontex, uv.xy);
//		search1 =mix( (frag.y - startfrag.y) / deltaY ,
//					(frag.x - startfrag.x) / deltaX, useX);
//		search1 = clamp(search1, 0.0, 1.0);
//		viewDistance = (posvs.y * endvs.y) / mix(endvs.y, posvs.y, search1);
//		depth = viewDistance - curpos.y;
//
//		if (depth > 0 && depth < thickness) {
//			hit0 = 1;
//			break;
//		} else {
//			search0 = search1;
//		}
//	}
//
//	search1 = search0 + ((search1 - search0) / 2.0);
//	steps *= hit0;
//
//	for (i = 0; i < steps; ++i) {
//		frag = mix(startfrag.xy, endfrag.xy, search1);
//		uv.xy      = frag / texSize;
//		curpos = texture(positiontex, uv.xy);
//		viewDistance = (posvs.y * endvs.y) / mix(endvs.y, posvs.y, search1);
//		depth = viewDistance - curpos.y;
//
//		if (depth > 0 && depth < thickness) {
//			hit1 = 1;
//			search1 = search0 + ((search1 - search0) / 2);
//		} else {
//			float temp = search1;
//			search1 = search1 + ((search1 - search0) / 2);
//			search0 = temp;
//		}
//	}
//
//	float visibility = hit1 * curpos.w * ( 1 - max(dot(- normalize(posvs.xyz), reflectVec), 0))
//    * ( 1- clamp( depth / thickness, 0, 1))
//    * ( 1- clamp( length(curpos - posvs) / maxDist, 0, 1))
//    * (uv.x < 0 || uv.x > 1 ? 0 : 1)
//    * (uv.y < 0 || uv.y > 1 ? 0 : 1);
//
//	visibility = clamp(visibility, 0, 1);
//	uv.ba = vec2(visibility);
//	vec3 co = texture(colortex,uv.xy).rgb;
//
//	fragmentColor.rgb =vec3(co);
//	fragmentColor.a =1.0f;