#include "DataStructures.h"



namespace Sampler {

	class Harmonics
	{
	public:
		Harmonics(int degree);
		void Evaluate(const std::vector<Vertex>& vertices);
		std::vector<glm::vec3> getCoefficients()const
		{
			return coefs;
		}
		glm::vec3 Render(const glm::vec3& pos);
		
	private:
		int degree_;
		std::vector<glm::vec3> coefs;

		std::vector<float> Basis(const glm::vec3& pos);
		std::vector<float> factorial;
	};
}