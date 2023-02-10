#include "pch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


namespace Mint {

	unsigned int GetShaderType(ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderType::VETEX_SHADER: return GL_VERTEX_SHADER;
		case ShaderType::FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;
		default:
		{
			MT_ASSERT(false, "Unknonw Shader Type");
			return 0;
		}
		}
	}

	static void CreateCacheDirectoryIfNeeded()
	{
		//toimplement file system 
		//std::string cacheDirectory = GetCacheDirectory();
		/*if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directories(cacheDirectory);*/
	}

	static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
		case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
		}
		MT_ASSERT(false);
		return "";
	}
	OpenGLShader::OpenGLShader(const std::vector<ShaderInfo>shaderInfos, const std::string& name)
		: m_Name(name),m_RendererID(-1)
	{
		GLint shaderprogram = glCreateProgram();
		std::vector<GLuint> shaderIds;
		shaderIds.reserve(shaderInfos.size());
		for (auto shaderinfo : shaderInfos) {
			GLuint shader = glCreateShader(GetShaderType(shaderinfo.type));
			shaderIds.emplace_back(shader);
			auto src = ReadFile(shaderinfo.path);
			const char* srccode = src.c_str();
			MT_ASSERT(srccode,"Shader Source in OpenGLShader is null");
			glShaderSource(shader, 1, &srccode, NULL);
			glCompileShader(shader);

			if (!CheckCompileError(shader, CompileType::LINK_STATUS))
			{
				for (auto& s : shaderIds)
				{
					glDeleteShader(s);
				}
				MT_ASSERT(false, "Shader Compile Failed");
			}
			glAttachShader(shaderprogram, shader);	
		}
		glLinkProgram(shaderprogram);

		if (!CheckCompileError(shaderprogram, CompileType::PROGRAM_STATUS))
		{
			for (auto& shader : shaderIds)
			{
				glDeleteShader(shader);
			}
			MT_ASSERT(false, "Program Compile Failed");
		}

		for (auto& shader : shaderIds)
		{
			glDeleteShader(shader);
		}

		m_RendererID = shaderprogram;
	}

	Mint::OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Mint::OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Mint::OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	bool OpenGLShader::CheckCompileError(unsigned int shaderprogram, CompileType type)
	{
		
		switch (type)
		{
		case Mint::CompileType::LINK_STATUS:
		{
			GLint iComplied(-1);
			glGetShaderiv(shaderprogram, GL_COMPILE_STATUS, &iComplied);
			if (!iComplied)
			{
				GLsizei uiLen(0);
				glGetShaderiv(shaderprogram, GL_INFO_LOG_LENGTH, &uiLen);

				std::vector<GLchar> logInfo(uiLen);
				glGetShaderInfoLog(shaderprogram, uiLen, &uiLen, &logInfo[0]);
				MT_ERROR(logInfo.data());
				return false;
			}
			return true;
		}
		case Mint::CompileType::PROGRAM_STATUS:
		{
			GLint iComplied(-1);
			glGetShaderiv(shaderprogram, GL_LINK_STATUS, &iComplied);
			if (!iComplied)
			{
				GLsizei uiLen(0);
				glGetShaderiv(shaderprogram, GL_INFO_LOG_LENGTH, &uiLen);

				std::vector<GLchar> logInfo(uiLen);
				glGetShaderInfoLog(shaderprogram, uiLen, &uiLen, &logInfo[0]);
				MT_ERROR(logInfo.data());
				return false;
			}
			return true;
		}
		default:
		{
			MT_ASSERT(false, "Unknow Complie Tye");
			return false;
		}
		}
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void Mint::OpenGLShader::SetInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void Mint::OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void Mint::OpenGLShader::SetFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void Mint::OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void Mint::OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Mint::OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void Mint::OpenGLShader::SetMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Mint::OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}