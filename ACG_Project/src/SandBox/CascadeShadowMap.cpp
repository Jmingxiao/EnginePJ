#include "pch.h"
#include "CascadeShadowMap.h"
#include "OpenGL/Renderer.h"

namespace Acg {
	CSM::CSM(uint32_t resolution, const Light& light)
		:shadowmapResolution(resolution),m_light(light)
	{
		std::vector<ShaderInfo> depthShaderInfo
		{
			{ShaderType::VETEX_SHADER,"assets/shaders/cascadeSM/depthcascade.vert"},
			{ShaderType::FRAGMENT_SHADER,"assets/shaders/cascadeSM/depthcascade.frag"},
			{ShaderType::GEOMETRY_SHADER,"assets/shaders/cascadeSM/depthcascade.gs"}
		};
		depthShader = CreateRef<Shader>(depthShaderInfo, "depthShader");


		glGenFramebuffers(1, &lightFBO);

		glGenTextures(1, &lightDepthMaps);
		glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
		glTexImage3D(
			GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, shadowmapResolution, shadowmapResolution, int(shadowCascadeLevels.size()) + 1,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
			throw 0;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenBuffers(1, &matricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 8, nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		

	}

    void CSM::Bind(const Ref<Camera>& cam)
    {
		lightMatrices = GetLightSpaceMatrices(cam);
		glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
		for (size_t i = 0; i < lightMatrices.size(); ++i)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		depthShader->Bind();
		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		RendererCommand::SetViewport(0, 0, shadowmapResolution, shadowmapResolution);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
    }

	void CSM::Unbind()
	{
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    void CSM::DebugLightFrustume(int level)
    {
		GLuint visualizerVAO;
		GLuint visualizerVBO;
		GLuint visualizerEBO;

		const GLuint indices[] = {
			0, 2, 3,
			0, 3, 1,
			4, 6, 2,
			4, 2, 0,
			5, 7, 6,
			5, 6, 4,
			1, 3, 7,
			1, 7, 5,
			6, 7, 3,
			6, 3, 2,
			1, 5, 4,
			0, 1, 4
		};

		const auto corners = Camera::GetFrustumCorners(lightMatrices[level]);
		std::vector<glm::vec3> vec3s;
		for (const auto& v : corners)
		{
			vec3s.push_back(glm::vec3(v));
		}

		glGenVertexArrays(1, &visualizerVAO);
		glGenBuffers(1, &visualizerVBO);
		glGenBuffers(1, &visualizerEBO);

		glBindVertexArray(visualizerVAO);

		glBindBuffer(GL_ARRAY_BUFFER, visualizerVBO);
		glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(visualizerVAO);
		glDrawElements(GL_LINE_STRIP, GLsizei(36), GL_UNSIGNED_INT, 0);

		glDeleteBuffers(1, &visualizerVBO);
		glDeleteBuffers(1, &visualizerEBO);
		glDeleteVertexArrays(1, &visualizerVAO);

		glBindVertexArray(0);
    }

    std::vector<glm::mat4> CSM::GetLightSpaceMatrices(const Ref<Camera>& cam)
    {
		std::vector<glm::mat4> ret;
		for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
		{
			if (i == 0)
			{
				ret.push_back(GetLightSpaceMatrix(cam, cam->GetNearPlane(), shadowCascadeLevels[i]));
			}
			else if (i < shadowCascadeLevels.size())
			{
				ret.push_back(GetLightSpaceMatrix(cam, shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
			}
			else
			{
				ret.push_back(GetLightSpaceMatrix(cam, shadowCascadeLevels[i - 1], cam->GetFarPlane()));
			}
		}
		return ret;
    }

    glm::mat4 CSM::GetLightSpaceMatrix(const Ref<Camera>& cam, const float nearPlane, const float farPlane)
    {
		const auto proj = glm::perspective(
			glm::radians(cam->GetFov()), cam->GetAspect(), nearPlane, farPlane);

		auto corners = Camera::GetFrustumCorners(proj * cam->GetViewMatrix());
		glm::vec3 center = glm::vec3(0, 0, 0);
		for (const auto& v : corners)
		{
			center += glm::vec3(v);
		}
		center /= corners.size();

		auto lightDir = glm::normalize(m_light.direction);
		glm::vec3 forward = glm::normalize(-lightDir);
		glm::vec3 right = forward.x != 0.0 && forward.z != 0 ?
			glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, .0f))) : glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		glm::mat4 lightView = glm::lookAt(center + lightDir, center, up);

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::lowest();
		for (const auto& v : corners)
		{
			glm::vec4 trf = lightView * v;
			minX = std::min(minX, trf.x);
			maxX = std::max(maxX, trf.x);
			minY = std::min(minY, trf.y);
			maxY = std::max(maxY, trf.y);
			minZ = std::min(minZ, trf.z);
			maxZ = std::max(maxZ, trf.z);
		}

		constexpr float zMult =10.0f;
		if (minZ < 0)
		{
			minZ *= zMult;
		}
		else
		{
			minZ /= zMult;
		}
		if (maxZ < 0)
		{
			maxZ /= zMult;
		}
		else
		{
			maxZ *= zMult;
		}

		const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		return lightProjection * lightView;
    }
}