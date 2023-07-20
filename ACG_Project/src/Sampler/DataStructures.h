#pragma once

#define PI 3.14159265358f

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>

namespace Sampler {

	struct Vertex {
		glm::vec3 pos{}, color{};
	};

	inline std::string CoefficientsString(const std::vector<glm::vec3>& coefs) {
		std::stringstream stream;
		for (auto& c : coefs) {
			stream << std::fixed << std::setprecision(6) << c.r << "\t"
				<<c.g<<"\t"<<c.b << std::endl;
		}
		return stream.str();
	}

	inline void WriteString2File(const std::string& filename, const std::string& content) {
		using namespace std;
		fstream file;
		file.open(filename, ios_base::out);

		if (!file.is_open())
			LOG_WARN("file Does not exist");

		file.write(content.data(), content.size());

		file.close();
	}

	inline std::vector<glm::vec3> ReadFloatFromFile(const std::string& filename) {
		using namespace std;
		std::ifstream stream;
		stream.open(filename);
		if (!stream.is_open())
			return{};
		vector<glm::vec3> coefs;
		string line;
		while (getline(stream, line))
		{
			stringstream ss(line);
			vector<string> words;
			string word;
			while (ss >> word) words.push_back(word);
			if (words.size() != 3)continue;
			auto coef = glm::vec3(std::stof(words[0]), std::stof(words[1]), std::stof(words[2]));
			coefs.push_back(coef);
		}
		stream.close();
		return coefs;
	}

	inline glm::vec2 Cartesian2Spherical(const  glm::vec3& p)
	{
		auto theta = std::acos(std::max(-1.0f, std::min(1.0f, p.y)));
		auto phi = std::atan2(p.z, p.x);
		if (phi < 0.0f)
			phi = phi + 2.0f * PI;
		return { phi / (2.0 * PI), 1-theta / PI };
	}	

	inline glm::vec3 Spherical2Cartesian(const  glm::vec3& s)
	{
		glm::vec3 p;
		auto phi = s.x * 2 * PI;
		auto t = s.y * PI;
		p.x = s.r * std::sin(t) * std::cos(phi);
		p.y = s.r * std::sin(t) * std::sin(phi);
		p.z = s.r * std::cos(t);
		return p;
	}

	inline float UniformRandom()
	{
		static std::default_random_engine generator;
		static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		return distribution(generator);
	}

	inline float NormalRandom(float mu = 0.f, float sigma = 1.f)
	{
		static std::default_random_engine generator;
		static std::normal_distribution<float> distribution(mu, sigma);
		return distribution(generator);
	}
}