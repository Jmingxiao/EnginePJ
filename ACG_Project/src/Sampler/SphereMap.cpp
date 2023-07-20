#include "pch.h"
#include "SphereMap.h"

using namespace Acg;
using namespace glm;
namespace Sampler {

SphericalMap::SphericalMap(const Ref<TextureHDR>& p_texture)
{
	texture = p_texture;
	img = texture->GetImgData();
	width = texture->GetWidth();
	height = texture->GetHeight();
}

std::vector<Vertex> Sampler::SphericalMap::getVertices()
{
	std::vector<Vertex> vertices(width * height);
	int i = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			
			float  u = (x + 0.5f) / width;
			float  v = (y + 0.5f) / height;
			auto color = Sample({u,v});
			vec3 pos;
			float phi = u*2 * PI;
			float theta = (1-v) * PI;
			pos = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
			vertices[i] = { pos,color };
			i++;
		}
	}
	return vertices;
}

std::vector<Sampler::Vertex> SphericalMap::RandomSample(int n)
{
	std::vector<Vertex> samples(n);
	for (int i = 0; i < n; i++)
	{
		float x, y, z;
		do {
			x = NormalRandom();
			y = NormalRandom();
			z = NormalRandom();
		} while (x == 0 && y == 0 && z == 0);
		vec3 p = normalize(vec3( x, y, z ));
		auto uv = Cartesian2Spherical(p);
		vec3 c = Sample(uv);
		//tone mapping
		vec3 mapped = c*PI;
		mapped= pow(mapped, vec3(1.0f / 2.2f));
		//mapped= vec3(1.0) - exp(-mapped * 5.0f);
		//std::cout << c << std::endl;
		samples[i] = { p, mapped};
	}
	return samples;
}

glm::vec3 SphericalMap::Sample(const glm::vec2& uv)
{ 
	if (img.size()!= 0) {

		int x = static_cast<int>(uv.x * (float)width);
		int y = static_cast<int>(uv.y * (float)height);
		int d = (y * width + x)*3;
		return vec3(img[d], img[d + 1], img[d + 2]);//texture->Sample(uv);
	}
	else {
		LOG_ERROR("img bad data");
		return {};
	}
}
}
