#version 420

#define SIGMA 10.0
#define BSIGMA 0.3
#define MSIZE 15
// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;
layout(binding = 0) uniform sampler2D colortex;

in vec2 texCoord;

float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

float normpdf3(in vec3 v, in float sigma)
{
	return 0.39894*exp(-0.5*dot(v,v)/(sigma*sigma))/sigma;
}

void main()
{
	vec2 texSize = textureSize(colortex, 0);
	vec4 c = texture(colortex,texCoord);
	const int kSize = (MSIZE-1)/2;
	float kernel[MSIZE];
	vec3 final_colour = vec3(0.0);
	
	//create the 1-D kernel
	float Z = 0.0;
	for (int j = 0; j <= kSize; ++j)
	{
		kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), SIGMA);
	}
	vec3 cc;
	float factor;
	float bZ = 1.0/normpdf(0.0, BSIGMA);
	//read out the texels
	for (int i=-kSize; i <= kSize; ++i)
	{
		for (int j=-kSize; j <= kSize; ++j)
		{
			cc = texture(colortex, texCoord+(vec2(float(i),float(j))/ texSize.xy)).rgb;
			factor = normpdf3(cc-c.rgb, BSIGMA)*bZ*kernel[kSize+j]*kernel[kSize+i];
			Z += factor;
			final_colour += factor*cc;
		}
	}
	
	fragmentColor = vec4(c);
	fragmentColor = vec4(final_colour/Z, c.a);

}