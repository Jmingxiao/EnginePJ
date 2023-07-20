#include "pch.h"
#include "Harmonics.h"


namespace Sampler {
	using namespace std;
	using namespace glm;

    Harmonics::Harmonics(int degree)
		:degree_(degree)
    {
		int maxfact = degree * degree;
		factorial.resize(maxfact + 1);
		for (int i = 0; i <= maxfact; i++)
		{
			if (i == 0)
				factorial[i] = 1;
			else
				factorial[i] = i * factorial[i - 1];
		}
    }

    void Harmonics::Evaluate(const std::vector<Vertex>& vertices)
    {
		int n = (degree_ + 1) * (degree_ + 1);
		coefs = vector<glm::vec3>(n, glm::vec3());
		for (const Vertex& v : vertices)
		{
			vector<float> Y = Basis(v.pos);
			for (int i = 0; i < n; i++)
			{
				coefs[i] += Y[i] *v.color;
			}
		}
		for (auto& coef : coefs)
		{
			coef *= 4 * PI / (float)vertices.size();
		}
    }

    glm::vec3 Harmonics::Render(const glm::vec3& pos)
    {
        return glm::vec3();
    }


    std::vector<float> Harmonics::Basis(const glm::vec3& pos)
    {
		int n = (degree_ + 1) * (degree_ + 1);
		std::vector<float> Y(n);
		glm::vec3 normal = normalize(pos);
		float x = normal.x;
		float y = normal.y;
		float z = normal.z;

		if (degree_ >= 0)
		{
			Y[0] = 1.f / 2.f * sqrt(1.f / PI);
		}
		if (degree_ >= 1)
		{
			Y[1] = sqrt(3.f / (4.f * PI)) * z;
			Y[2] = sqrt(3.f / (4.f * PI)) * y;
			Y[3] = sqrt(3.f / (4.f * PI)) * x;
		}
		if (degree_ >= 2)
		{
			Y[4] = 1.f / 2.f * sqrt(15.f / PI) * x * z;
			Y[5] = 1.f / 2.f * sqrt(15.f / PI) * z * y;
			Y[6] = 1.f / 4.f * sqrt(5.f / PI) * (-x * x - z * z + 2 * y * y);
			Y[7] = 1.f / 2.f * sqrt(15.f / PI) * y * x;
			Y[8] = 1.f / 4.f * sqrt(15.f / PI) * (x * x - z * z);
		}
		if (degree_ >= 3)
		{
			Y[9] = 1.f / 4.f * sqrt(35.f / (2.f * PI)) * (3 * x * x - z * z) * z;
			Y[10] = 1.f / 2.f * sqrt(105.f / PI) * x * z * y;
			Y[11] = 1.f / 4.f * sqrt(21.f / (2.f * PI)) * z * (4 * y * y - x * x - z * z);
			Y[12] = 1.f / 4.f * sqrt(7.f / PI) * y * (2 * y * y - 3 * x * x - 3 * z * z);
			Y[13] = 1.f / 4.f * sqrt(21.f / (2.f * PI)) * x * (4 * y * y - x * x - z * z);
			Y[14] = 1.f / 4.f * sqrt(105.f / PI) * (x * x - z * z) * y;
			Y[15] = 1.f / 4.f * sqrt(35.f / (2 * PI)) * (x * x - 3 * z * z) * x;
		}
		return Y;
    }


}