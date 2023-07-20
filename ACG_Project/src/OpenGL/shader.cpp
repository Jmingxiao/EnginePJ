#include "pch.h"
#include "shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Acg {

    unsigned int GetShaderType(ShaderType shaderType)
    {
        switch (shaderType)
        {
        case ShaderType::VETEX_SHADER: return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;
        case ShaderType::GEOMETRY_SHADER: return GL_GEOMETRY_SHADER;
        default:
        {
            LOG_ASSERT(false, "Unknonw Shader Type");
            return 0;
        }
        }
    }

    Shader::Shader(const std::vector<ShaderInfo> shaderInfos, const std::string& name)
        : m_Name(name), m_RendererID(-1)
    {
        GLint shaderprogram = glCreateProgram();
        std::vector<GLuint> shaderIds;
        shaderIds.reserve(shaderInfos.size());
        for (auto shaderinfo : shaderInfos) {
            GLuint shader = glCreateShader(GetShaderType(shaderinfo.type));
            shaderIds.emplace_back(shader);
            auto src = ReadFile(shaderinfo.path);
            const char* srccode = src.c_str();
            LOG_ASSERT(srccode, "Shader Source in OpenGLShader is null");
            glShaderSource(shader, 1, &srccode, NULL);
            glCompileShader(shader);

            if (!CheckCompileError(shader, CompileType::LINK_STATUS))
            {
                for (auto& s : shaderIds)
                {
                    glDeleteShader(s);
                }
                LOG_ASSERT(false, "Shader Compile Failed");
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
            LOG_ASSERT(false, "Program Compile Failed");
        }

        for (auto& shader : shaderIds)
        {
            glDeleteShader(shader);
        }

        m_RendererID = shaderprogram;
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_RendererID);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    bool Shader::CheckCompileError(unsigned int shaderprogram, CompileType type)
    {
        switch (type)
        {
        case CompileType::LINK_STATUS:
        {
            GLint iComplied(-1);
            glGetShaderiv(shaderprogram, GL_COMPILE_STATUS, &iComplied);
            if (!iComplied)
            {
                GLsizei uiLen(0);
                glGetShaderiv(shaderprogram, GL_INFO_LOG_LENGTH, &uiLen);

                std::vector<GLchar> logInfo(uiLen);
                glGetShaderInfoLog(shaderprogram, uiLen, &uiLen, &logInfo[0]);
                LOG_ERROR(logInfo.data());
                return false;
            }
            return true;
        }
        case CompileType::PROGRAM_STATUS:
        {
            GLint iComplied(-1);
            glGetShaderiv(shaderprogram, GL_LINK_STATUS, &iComplied);
            if (!iComplied)
            {
                GLsizei uiLen(0);
                glGetShaderiv(shaderprogram, GL_INFO_LOG_LENGTH, &uiLen);

                std::vector<GLchar> logInfo(uiLen);
                glGetShaderInfoLog(shaderprogram, uiLen, &uiLen, &logInfo[0]);
                LOG_ERROR(logInfo.data());
                return false;
            }
            return true;
        }
        default:
        {
            LOG_ASSERT(false, "Unknow Complie Type");
            return false;
        }
        }
    }

    void Shader::SetBool(const std::string& name, bool value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::SetInt(const std::string& name, int value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void Shader::SetFloat(const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void Shader::SetMat3(const std::string& name, const glm::mat3& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    std::string Shader::ReadFile(const std::string& filepath)
    {
        std::ifstream ifs;
        std::string content;
        ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            ifs.open(filepath);
            std::stringstream ss;
            ss << ifs.rdbuf();
            ifs.close();
            content = ss.str();
        }
        catch (std::ifstream::failure& e)
        {
            LOG_ERROR("Could not open file '{0}'", filepath);
            LOG_ERROR("Description: '{0}'", e.what());
            LOG_ERROR("Error code: '{0}'", e.code());
            return "";
        }
        return content;
    }

    void ShaderLibrary::Init()
    {
        std::vector<ShaderInfo> simpleShaderInfo
        {
            {ShaderType::VETEX_SHADER,"assets/shaders/color.vert"},
            {ShaderType::FRAGMENT_SHADER,"assets/shaders/color.frag"}
        };
        std::vector<ShaderInfo> pbrShaderInfo
        {
            {ShaderType::VETEX_SHADER,"assets/shaders/pbr.vert"},
            {ShaderType::FRAGMENT_SHADER,"assets/shaders/pbr.frag"}
        };
        std::vector<ShaderInfo> backgroundShaderInfo
        {
            {ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
            {ShaderType::FRAGMENT_SHADER,"assets/shaders/background.frag"}
        };
        simple = CreateRef<Shader>(simpleShaderInfo, "simple");
        pbr = CreateRef<Shader>(pbrShaderInfo, "pbr");
        background = CreateRef<Shader>(backgroundShaderInfo, "background");
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        LOG_ASSERT(!Exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();
        Add(name, shader);
    }

    Ref<Shader> ShaderLibrary::Load(const std::vector<ShaderInfo>& vecShaderInfo, const std::string& name)
    {
        auto shader = CreateRef<Shader>(vecShaderInfo, name);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        LOG_ASSERT(Exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    Ref<Shader> ShaderLibrary::GetDefaultShader(BuiltinShaderType type)
    {
        switch (type)
        {
        case BuiltinShaderType::simple: return simple;
        case BuiltinShaderType::pbr: return pbr;
        case BuiltinShaderType::background: return background;
        }
        LOG_ASSERT(false, "Not A Deafult Shader Type!!!");
        return nullptr;
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }

}