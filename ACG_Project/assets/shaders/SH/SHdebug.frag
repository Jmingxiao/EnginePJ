#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;


///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359


layout(location = 0) out vec4 fragmentColor;
layout(binding = 0) uniform sampler2D environmentMap;

in vec2 texCoord;

uniform vec3 coefs[16];
uniform int degree_;

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


void main()
{             
	float phi = (texCoord.x)*2*PI;
	float theta = (1-texCoord.y) * PI;
	vec3 n = vec3(sin(theta)*cos(phi),cos(theta),sin(theta)*sin(phi));
	
	theta = acos(max(-1.0f, min(1.0f, n.y)));
	 phi = atan(n.z, n.x);
	if(phi < 0.0f)
		phi = phi + 2.0f * PI;

	// Use these to lookup the color in the environment map
	vec2 lookup = vec2(phi / (2.0 * PI), 1 - theta / PI);


	int camount = (degree_ + 1)*(degree_ + 1);//numbers of coefficiencys 
	vec3 color =vec3(0.0f);
	float[16] baseY;
	GetBasis(baseY,n);

	for (int i = 0; i < camount; i++)
	{
		color += baseY[i]*coefs[i];
	}

    fragmentColor =vec4(color,1.0f); 
	//fragmentColor = texture2D(environmentMap,lookup);
}