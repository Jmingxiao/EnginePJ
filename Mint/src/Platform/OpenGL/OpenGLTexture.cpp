#include "pch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"


Mint::OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t n_component)
    :m_Width(width), m_Height(height), m_components(n_component)
{
    Setfomat(n_component);
    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, width, height, 0, m_format, GL_UNSIGNED_BYTE, nullptr);

    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Mint::OpenGLTexture2D::OpenGLTexture2D(const std::string& path, uint32_t n_component)
    : m_Path(path), m_components(n_component)
{   
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = nullptr;
    {
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    }
    if (data)
    {
        m_IsLoaded = true;
        m_data = data;
        m_Width = width;
        m_Height = height;

        Setfomat(n_component);
        MT_ASSERT(m_format & m_internalformat, "Format not supported!");


        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_internalformat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }else{
         MT_ERROR("bad data!!")
    }
}

Mint::OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_RendererID);
}

const std::string Mint::OpenGLTexture2D::GetFileName() const
{
    auto filename = file::normalise(m_Path);
    return file::file_stem(filename);
}

void Mint::OpenGLTexture2D::SetData(void* data, uint32_t size)
{
    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, m_Width, m_Width, 0, m_format, GL_UNSIGNED_BYTE, data);
}

void Mint::OpenGLTexture2D::SetFilter(uint32_t filter, uint32_t address)
{
    Bind();
    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter);

    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, address);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, address);
}

void Mint::OpenGLTexture2D::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, m_RendererID);
}

glm::vec4 Mint::OpenGLTexture2D::Sample(glm::vec2 uv) const
{
    if (m_data)
    {
        int x = int(uv.x * m_Width + 0.5) % m_Width;
        int y = int(uv.y * m_Height + 0.5) % m_Height;
        if (m_components == 4)
        {
            return glm::vec4(m_data[(y * m_Width + x) * 4 + 0], m_data[(y * m_Width + x) * 4 + 1],
                m_data[(y * m_Width + x) * 4 + 2], m_data[(y * m_Width + x) * 4 + 3])
                / 255.f;
        }
        else
        {
            // Just return one channel
            return glm::vec4(m_data[(y * m_Width + x) * m_components + 0], m_data[(y * m_Width + x) * m_components + 0],
                m_data[(y * m_Width + x) * m_components + 0], m_data[(y * m_Width + x) * m_components + 0])
                / 255.f;
        }
    }else{
        MT_ERROR("bad data!!!");
        return {};
    }
}
