#pragma once

#include "Mint/Render/Shader.h"
#include <glm/glm.hpp>

namespace Mint {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::vector<ShaderInfo>shaderInfos, const std::string& name);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		bool CheckCompileError(unsigned int shaderprogram, CompileType type);

		virtual void SetBool(const std::string& name, bool value) override;
		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat3(const std::string& name, const glm::mat3& matrix) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;

		virtual const std::string& GetName() const override { return m_Name; }
		

	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::unordered_map<unsigned int, std::string> m_OpenGLSourceCode;



	};
}
