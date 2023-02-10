#include "pch.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "RenderAPI.h"

namespace Mint {

    Ref<Shader> Shader::Create(const std::vector<ShaderInfo>& vecShaderInfo, const std::string& name)
    {
        switch (RenderAPI::GetAPI())
        {
        case RenderAPI::API::OpenGL:
            return Ref<OpenGLShader>(new OpenGLShader(vecShaderInfo, name));
        case RenderAPI::API::None:    MT_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        }
        MT_ASSERT(false, "Unknown RenderAPI");
        return nullptr;
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
            MT_ERROR("Could not open file '{0}'", filepath);
            MT_ERROR("Description: '{0}'",e.what() );
            MT_ERROR("Error code: '{0}'", e.code() );
            return "";
        }
        return content;
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        MT_ASSERT(!Exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();
        Add(name, shader);
    }

    Ref<Shader> ShaderLibrary::Load(const std::vector<ShaderInfo>& vecShaderInfo, const std::string& name)
    {
        auto shader = Shader::Create(vecShaderInfo,name);
        Add(shader);
        return shader;
    }


    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        MT_ASSERT(Exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
}


