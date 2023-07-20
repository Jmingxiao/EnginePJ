#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;


///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359

///////////////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////////////
uniform vec3 material_color;
uniform float material_metalness;
uniform float material_fresnel;
uniform float material_shininess;
uniform vec3 material_emission;

uniform int has_color_texture;
layout(binding = 0) uniform sampler2D colorMap;
uniform int has_emission_texture;
layout(binding = 5) uniform sampler2D emissiveMap;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
uniform float environment_multiplier;
uniform vec3 coefs[16];
uniform int degree_;


///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
uniform vec3 lightcolor;
uniform vec3 lightDirection;//viewspace
uniform float lightIntensity;

///////////////////////////////////////////////////////////////////////////////
// Input varyings from vertex shader
///////////////////////////////////////////////////////////////////////////////
in vec2 texCoord;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
in mat4 viewInverse;

///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////


layout(location = 0) out vec4 fragmentColor;

float roughness;
float ss =0.0;

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


	///////////////////////////////////////////////////////////////////////////
	// Task 1.3 - Calculate the diffuse term and return that as the result
	// instead of lambertian diffuse I chose disney diffuse
	// Disney diffuse:https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
	// git hub£º https://github.com/wdas/brdf/blob/main/src/brdfs/disney.brdf
	///////////////////////////////////////////////////////////////////////////

	vec3 lightcolor = lightIntensity*lightcolor;

	float Fd90 = 0.5 + 2.0*roughness*lh*lh;
    float fdlight = pow(1.0-nl,5.0);
    float fdview = pow(1.0-nv,5.0);
    float fd = (1.0+(Fd90-1.0)*fdlight) *(1.0+(Fd90-1)*fdview);
	vec3 basediffuse =	base_color/PI*nl*lightcolor;
	
	//subsurface diffuse disney 2015 bidirectional subsurface scattering
	vec3 ssdiffuse;
	float Fss90 = roughness*lh*lh;
	float fslight = (1.0+(Fss90-1.0)*pow(1.0-nl,5.0));
	float fsview = (1.0+(Fss90-1.0)*pow(1.0-nv,5.0));
	float fss = fslight*fsview;
	ssdiffuse = 1.25*direct_illum/PI*(fss*(1/(nl+nv)-0.5)+0.5)*nl*lightcolor;

	vec3 dielectricdiff = mix(basediffuse,ssdiffuse,ss);


	///////////////////////////////////////////////////////////////////////////
	// Task 2 - Calculate the Torrance Sparrow BRDF and return the light
	//          reflected from that instead
	///////////////////////////////////////////////////////////////////////////
	
	// schlick fresnel term
	float t = pow(1.0-lh,5.0);
    float F =material_fresnel+(1.0-material_fresnel)*t;

	//blinnphong 

	float D = pow(nh,material_shininess)*(material_shininess+2)/(2*PI);

	float G = min(1,min(2*nh*nv/vh,2*nh*nl/vh));

	///////////////////////////////////////////////////////////////////////////
	// Task 3 - Make your shader respect the parameters of our material model.
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


vec3 calculateIndirectIllumination(vec3 viewDirVS, vec3 n, vec3 base_color)
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
	/////////////////////////////////////////////////////////////////////////
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
	vec3 indirectspec = vec3(1.0);
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
	indirectspec =vec3(.0f);

	vec3 dielectric  = indirectspec*F+ (1-F)* diffuse;
	vec3 metalic = F*base_color*indirectspec;

	indirect_illum = dielectric*(1-material_metalness)+metalic*material_metalness;
	return diffuse;
}

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}


void main()
{
	vec3 viewpos = vec3(0.0);
	vec3 viewDir = normalize( viewpos- viewSpacePosition);
	vec3 normalvs = normalize(viewSpaceNormal);
	roughness = sqrt(sqrt(2/(material_shininess+2)));


	vec3 base_color = material_color;
	if(has_color_texture == 1)
	{
		vec4 color = texture(colorMap, texCoord);
		if(color.a<=0.3f)
			discard;
		base_color =color.rgb;
	}
	base_color =vec3(1.0f);
	vec3 direct_illumination_term = vec3(0.0);
	{ // Direct illumination
		direct_illumination_term = calculateDirectIllumiunation(viewDir, normalvs,base_color);
	}

	vec3 indirect_illumination_term = vec3(0.0);
	{ // Indirect illumination
		indirect_illumination_term = calculateIndirectIllumination(viewDir, normalvs, base_color);
	}

	///////////////////////////////////////////////////////////////////////////
	// Add emissive term. If emissive texture exists, sample this term.
	///////////////////////////////////////////////////////////////////////////
	vec3 emission_term = material_emission;
	if(has_emission_texture == 1)
	{
		emission_term *= texture(emissiveMap, texCoord).rgb;
	}

	fragmentColor.rgb = /*direct_illumination_term +*/ indirect_illumination_term + emission_term;
	fragmentColor.a =1.0f;

}