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
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
uniform float environment_multiplier;

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
uniform vec3 lightcolor;
uniform vec3 lightDirection;
uniform float lightIntensity;
uniform int entity_id;

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
layout(location = 1) out int entid;

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
    float fd = mix(1.0, Fd90, fdlight) * mix(1.0, Fd90, fdview);
	vec3 basediffuse =	base_color/PI*nl*lightcolor*fd;
	
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

	// normal distribution function and geomatry
	//GGX

//		float a = pow(roughness,2);
//		float under = pow(nh*nh*(a*a-1.0)+1.0,2.0)*PI;
//		float D = a*a/under;
//
//		float tan2thetav = (1-pow(nv,2))/pow(nv,2);
//		float tan2thetal = (1-pow(nl,2))/pow(nl,2);
//		float Xv = vh/nv>0? 1:0;
//		float Xl = lh/nl>0? 1:0;
//		float G1v = Xv *2/(1+sqrt(1+a*a*tan2thetav));
//		float G1l = Xl *2/(1+sqrt(1+a*a*tan2thetal));
//		float G = G1l*G1v;

	//blinnphong 

	float D = pow(nh,material_shininess)*(material_shininess+2)/(2*PI);

	float G = min(1,min(2*nh*nv/vh,2*nh*nl/vh));

	///////////////////////////////////////////////////////////////////////////
	// Task 3 - Make your shader respect the parameters of our material model.
	///////////////////////////////////////////////////////////////////////////

	float brdf = D*G*F/(4*nl*nv);

	vec3 dielectric = nl*brdf*lightcolor +(1-material_metalness)*dielectricdiff;
	vec3 matelic =  material_metalness*base_color*nl*lightcolor*brdf;
	direct_illum = matelic+dielectric;
	
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
	///////////////////////////////////////////////////////////////////////////
	vec3 irradiance = vec3(1.0);
	{
		float theta = acos(max(-1.0f, min(1.0f, normalws.y)));
		float phi = atan(normalws.z, normalws.x);
		if(phi < 0.0f)
		{
			phi = phi + 2.0f * PI;
		}
		vec2 normalsph = vec2(phi / (2.0 * PI), 1 - theta / PI);
		irradiance = texture2D(irradianceMap,normalsph).rgb;
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

	vec3 dielectric  = indirectspec*F+ (1-F)* diffuse;
	vec3 metalic = F*base_color*indirectspec;

	indirect_illum = dielectric*(1-material_metalness)+metalic*material_metalness;
	return indirect_illum;
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
		base_color *= texture(colorMap, texCoord).rgb;
	}

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

	fragmentColor.rgb = direct_illumination_term+ indirect_illumination_term + emission_term;
	fragmentColor.a =1.0f;

	entid = entity_id;
}