#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

///////////////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////////////
uniform vec3 material_color = vec3(1, 1, 1);
uniform float material_metalness = 0;
uniform float material_fresnel = 0;
uniform float material_shininess = 0;
uniform vec3 material_emission;

uniform int has_color_texture = 0;
layout(binding = 0) uniform sampler2D colorMap;
uniform int has_emission_texture;
layout(binding = 5) uniform sampler2D emissiveMap;

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
uniform vec3 lightcolor;
uniform vec3 lightDirection;//viewspace
uniform float lightIntensity;

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359

///////////////////////////////////////////////////////////////////////////////
// Input varyings from vertex shader
///////////////////////////////////////////////////////////////////////////////
in vec2 texCoord;
in mat4 viewInverse;
in vec3 viewSpaceNormal;
in vec4 viewSpacePosition;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D reflectionMap;
uniform float environment_multiplier =1.5f;
uniform vec3 coefs[16];
uniform int degree_;



///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 normalVS;
layout(location = 2) out vec4 positionVS;
layout(location = 3) out vec4 basecolor;
layout(location = 4) out vec4 diffuseOnly;





float roughness;

vec3 screenpos;

vec3 calculateDirectIllumiunation(vec3 viewDir, vec3 normalvs, vec3 base_color)
{
	vec3 direct_illum = base_color;
	vec3 lightDir = normalize(lightDirection);

	// prepared parameters
	float nv = max(clamp(dot(normalvs, viewDir),0.0,1.0), 0.000001);
	vec3 halfVector = normalize(lightDir + viewDir);
    float nl = max(clamp(dot(normalvs, lightDir),	0.0,1.0), 0.000001);
	float vh = max(clamp(dot(viewDir, halfVector),	0.0,1.0), 0.000001);
	float lh = max(clamp(dot(lightDir, halfVector),	0.0,1.0), 0.000001);
	float nh = max(clamp(dot(normalvs, halfVector),	0.0,1.0), 0.000001);

	vec3 lightcolor = lightIntensity*lightcolor;

	float Fd90 = 0.5 + 2.0*roughness*lh*lh;
    float fdlight = pow(1.0-nl,5.0);
    float fdview = pow(1.0-nv,5.0);
    float fd = (1.0+(Fd90-1.0)*fdlight) *(1.0+(Fd90-1)*fdview);
	vec3 basediffuse =	base_color/PI*nl*lightcolor;
	

	vec3 dielectricdiff = basediffuse;


	///////////////////////////////////////////////////////////////////////////
	// Calculate the Torrance Sparrow BRDF and return the light
	// reflected from that instead
	///////////////////////////////////////////////////////////////////////////
	
	// schlick fresnel term
	float t = pow(1.0-lh,5.0);
    float F =material_fresnel+(1.0-material_fresnel)*t;


	//blinnphong 

	float D = pow(nh,material_shininess)*(material_shininess+2)/(2*PI);

	float G = min(1,min(2*nh*nv/vh,2*nh*nl/vh));

	///////////////////////////////////////////////////////////////////////////
	// Make your shader respect the parameters of our material model.
	///////////////////////////////////////////////////////////////////////////

	float brdf = D*G*F/(4*nl*nv);

	vec3 dielectric = nl*brdf*lightcolor+dielectricdiff*(1-F);
	vec3 matelic =  material_metalness*base_color*nl*lightcolor*brdf;
	direct_illum = matelic+(1-material_metalness)*dielectric;
	
	return direct_illum;
}

vec2 worldtosph(vec3 dir){

	float theta = acos(max(-1.0f, min(1.0f, dir.z)));
	float phi = atan(dir.y, dir.x);
	if(phi < 0.0f)
		phi = phi + 2.0f * PI;
	// Use these to lookup the color in the environment map
	return vec2(phi / (2.0 * PI), theta / PI);
}

void GetBasis(inout float baseY[16], vec3 pos){
	vec3 normal = normalize(pos);
	float x = normal.x;
	float y = normal.y;
	float z = normal.z;

	if(degree_>= 0){
		baseY[0] = 1.f / 2.f * sqrt(1.f / PI);
	}
	if(degree_>= 1){
		baseY[1] = sqrt(3.f / (4.f*PI))*z;
		baseY[2] = sqrt(3.f / (4.f*PI))*y;
		baseY[3] = sqrt(3.f / (4.f*PI))*x;
	}
	if(degree_ >= 2){
		baseY[4] = 1.f / 2.f * sqrt(15.f / PI) * x * z;
		baseY[5] = 1.f / 2.f * sqrt(15.f / PI) * z * y;
		baseY[6] = 1.f / 4.f * sqrt(5.f / PI) * (-x * x - z * z + 2 * y*y);
		baseY[7] = 1.f / 2.f * sqrt(15.f / PI) * y * x;
		baseY[8] = 1.f / 4.f * sqrt(15.f / PI) * (x*x - z * z);
	}
	if(degree_>= 3){
		baseY[9] = 1.f / 4.f*sqrt(35.f / (2.f*PI))*(3 * x*x - z*z)*z;
		baseY[10] = 1.f / 2.f*sqrt(105.f / PI)*x*z*y;
		baseY[11] = 1.f / 4.f*sqrt(21.f / (2.f*PI))*z*(4 * y*y - x*x - z*z);
		baseY[12] = 1.f / 4.f*sqrt(7.f / PI)*y*(2 * y*y - 3 * x*x - 3 * z*z);
		baseY[13] = 1.f / 4.f*sqrt(21.f / (2.f*PI))*x*(4 * y*y - x*x - z*z);
		baseY[14] = 1.f / 4.f*sqrt(105.f / PI)*(x*x - z*z)*y;
		baseY[15] = 1.f / 4.f*sqrt(35.f / (2 * PI))*(x*x - 3 * z*z)*x;
	}
}

vec4 calculateIndirectIllumination(vec3 viewDirVS, vec3 n, vec3 base_color,inout vec3 diffuse_only)
{
	vec3 lightDir = normalize(lightDirection);
	vec3 indirect_illum = vec3(0.f);
	vec3 normalws = (viewInverse*vec4(n,0.0)).rgb;
	normalws = normalize(normalws);
	vec3 viewDir = (viewInverse*vec4(viewDirVS,0.0)).rgb;
	vec3 reflectVec = reflect(-viewDir, normalws);

	///////////////////////////////////////////////////////////////////////////
	//Lookup the irradiance from the irradiance map and calculate
	//the diffuse reflection
	///////////////////////////////////////////////////////////////////////////
	vec3 irradiance = vec3(0.0);
	{
		float[16] baseY;
		GetBasis(baseY,normalws);
		int camount = (degree_ + 1)*(degree_ + 1);
		for (int i = 0; i < camount; i++)
		{
			irradiance = irradiance + baseY[i]*coefs[i];
		}
	}
	vec3 diffuse = irradiance*base_color/PI;

	///////////////////////////////////////////////////////////////////////////
	// Look up in the reflection map from the perfect specular
	// direction and calculate the dielectric and metal terms.
	///////////////////////////////////////////////////////////////////////////
	vec3 indirectspec = vec3(0.0);
	{
		float theta = acos(max(-1.0f, min(1.0f, reflectVec.y)));
		float phi = atan(reflectVec.z, reflectVec.x);
		if(phi < 0.0f)
		{
			phi = phi + 2.0f * PI;
		}
		vec2 reflectsph = vec2(phi / (2.0 * PI), 1 - theta / PI);
		indirectspec = environment_multiplier*textureLod(reflectionMap,reflectsph,roughness*7.0).rgb;
	}

	vec3 halfVector = normalize(lightDir + viewDir);
	float lh = max(clamp(dot(lightDir, halfVector),	0.0,1.0), 0.000001);
	float t = pow(1.0-lh,5.0);
	float F =material_fresnel+(1.0-material_fresnel)*t;

	diffuse_only = (1-F)*diffuse*(1-material_metalness);
	vec3 dielectric  = indirectspec*F;
	vec3 metalic = F*base_color*indirectspec;
	indirect_illum = dielectric*(1-material_metalness)+metalic*material_metalness+diffuse_only;
	return vec4(indirect_illum,F);
}

void main()
{

	
	vec3 viewpos = vec3(0.0);
	vec3 viewDir = normalize( viewpos- viewSpacePosition.xyz);
	vec3 normalvs = normalize(viewSpaceNormal);
	roughness = sqrt(sqrt(2/(material_shininess+2)));

	vec3 base_color = material_color;
	if(has_color_texture == 1)
	{
		base_color = base_color * texture(colorMap, texCoord).rgb;
	}

	vec3 direct_illumination_term = vec3(0.0);
	{ // Direct illumination
		direct_illumination_term = calculateDirectIllumiunation(viewDir, normalvs,base_color);
	}

	float F;
	vec3 indirect_illumination_term = vec3(0.0);
	vec3 diffuse= vec3(0.0);
	{ // Indirect illumination
		vec4 temp =calculateIndirectIllumination(viewDir, normalvs, base_color,diffuse);
		indirect_illumination_term = temp.rgb;
		F = temp.a;
	}

	///////////////////////////////////////////////////////////////////////////
	// Add emissive term. If emissive texture exists, sample this term.
	///////////////////////////////////////////////////////////////////////////


	vec3 emission_term = material_emission;
	if(has_emission_texture == 1)
	{
		emission_term = vec3(texture(emissiveMap, texCoord).rgb);
	}

	vec3 shading =  emission_term+indirect_illumination_term+ direct_illumination_term;

	fragmentColor = vec4(shading, F);
	normalVS = vec4(normalvs,roughness);
	positionVS = viewSpacePosition;
	basecolor = vec4(base_color,material_metalness);
	diffuseOnly = vec4(diffuse+emission_term+direct_illumination_term,1);
	return;
}
