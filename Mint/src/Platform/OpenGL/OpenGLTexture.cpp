#include "pch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"
#include "stb_image_write.h"

namespace Mint {

    OpenGLTextureHDR::OpenGLTextureHDR(const std::string& path)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);

        float* data = nullptr;
        {
            data = stbi_loadf(path.c_str(), &width, &height, &channels, 3);
        }
        if (data)
        {
            m_IsLoaded = true;
            m_data = data;
            m_Width = width;
            m_Height = height;

            MT_ASSERT(m_format & m_internalformat, "Format not supported!");

            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, m_data);

            stbi_image_free(data);
        }
        else {
            MT_ERROR("bad data!!")
        }
    }

    Mint::OpenGLTextureHDR::OpenGLTextureHDR(const std::vector<std::string>& path)
    {
        int width, height, channels;
        float* data = nullptr;
        {
            data = stbi_loadf(path[0].c_str(), &width, &height, &channels, 3);
        }
        if (data)
        {
            m_IsLoaded = true;
            m_data = data;
            m_Width = width;
            m_Height = height;

            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // We call this again because AMD drivers have some weird issue in the GenerateMipmap function that
            // breaks the first level of the image.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
        }
        else {
            MT_ERROR("bad data!!")
        }

        const int roughnesses = 8;
        for (int i = 1; i < roughnesses; i++)
        {
            data = stbi_loadf(path[i].c_str(), &width, &height, &channels, 3);
            glTexImage2D(GL_TEXTURE_2D, i, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
        }

    }

    OpenGLTextureHDR::~OpenGLTextureHDR()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTextureHDR::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }

    void OpenGLTextureHDR::SaveHdrTexture(const std::string& path, uint32_t texture_id)
    {
        std::vector<float> img;
        std::vector<uint8_t> img_png;

        glBindTexture(GL_TEXTURE_2D, texture_id);

        GLint lwidth, lheight;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &lwidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &lheight);

        const int n_channels = 3;
        img.resize(lwidth * lheight * n_channels);
        img_png.resize(img.size());

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, img.data());

        for (int r = 0; r < lheight / 2; ++r)
        {
            int a = r * lwidth * n_channels;
            int b = (lheight - 1 - r) * lwidth * n_channels;
            for (int c = 0; c < lwidth * n_channels; ++c)
            {
                std::swap(img[a + c], img[b + c]);

                img_png[a + c] = uint8_t(255 * (img[a + c] / (1 + img[a + c])));
                img_png[b + c] = uint8_t(255 * (img[b + c] / (1 + img[b + c])));
            }
        }

        stbi_write_hdr((path + ".hdr").c_str(), lwidth, lheight, 3, img.data());
        stbi_write_png((path + ".png").c_str(), lwidth, lheight, 3, img_png.data(), 0);
    }


    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t n_component)
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

    OpenGLTexture2D::OpenGLTexture2D(const std::string& path, uint32_t n_component)
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
        }
        else {
            MT_ERROR("bad data!!")
        }
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    const std::string OpenGLTexture2D::GetFileName() const
    {
        auto filename = file::normalise(m_Path);
        return file::file_stem(filename);
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size)
    {
        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, m_Width, m_Width, 0, m_format, GL_UNSIGNED_BYTE, data);
    }

    void OpenGLTexture2D::SetFilter(uint32_t filter, uint32_t address)
    {
        Bind();
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filter);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, address);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, address);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }

    glm::vec4 OpenGLTexture2D::Sample(glm::vec2 uv) const
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
        }
        else {
            MT_ERROR("bad data!!!");
            return {};
        }
    }

}