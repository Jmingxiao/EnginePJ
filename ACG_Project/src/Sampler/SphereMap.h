#pragma once

#include "Common/Common.h"
#include "DataStructures.h"
namespace Sampler {

class SphericalMap {
	Acg::Ref<Acg::TextureHDR> texture;
public:
	SphericalMap() = default;
	SphericalMap(const Acg::Ref<Acg::TextureHDR>& textureName);
	std::vector<Vertex> getVertices();
	std::vector<Vertex> RandomSample(int sqrt_n);
	glm::vec3 Sample(const glm::vec2& uv);

private:
	int width;
	int height; 
	std::vector<float> img;
};

}