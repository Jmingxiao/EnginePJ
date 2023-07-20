#pragma once

#include "Camera.h"
#include "Common/Common.h"


namespace Acg {

	struct Light {
		float intensity{ 2.0f };
		glm::vec3 direction{ glm::vec3(0.0f, 1.0f, 0.0f) };
		glm::vec3 color{ 1.0f };
		Model* light = nullptr;
	};

	class CSM
	{
	public:
		CSM() = default;
		CSM(uint32_t resolution, const Light& light = Light());
		void Bind(const Ref<Camera>& cam);
		void Unbind();
		void DebugLightFrustume(int level);
		std::vector<glm::mat4>  GetLightSpaceMatrices(const Ref<Camera>& cam);
		glm::mat4 GetLightSpaceMatrix(const Ref<Camera>& cam, const float nearPlane, const float farPlane);
		int GetLevelSize() { return (int)shadowCascadeLevels.size(); }
		float GetLevel(size_t i) { return shadowCascadeLevels[i]; }
		uint32_t GetshaowMapID() { return lightDepthMaps; }	

	public:
		Ref<Shader> depthShader;
		Light m_light;
		std::vector<float> shadowCascadeLevels{ 1000.0f / 50.0f, 1000.0f / 25.0f, 1000.0f / 10.0f, 1000.0f / 2.0f };
	private:

		Ref<Shader> debugCsmShader;

		uint32_t matricesUBO;
		uint32_t lightFBO;
		uint32_t lightDepthMaps;
		uint32_t shadowmapResolution = 4096;
		std::vector<glm::mat4>lightMatrices{};
	};
}