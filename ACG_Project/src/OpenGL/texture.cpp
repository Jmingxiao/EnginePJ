#include "pch.h"
#include "texture.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "Glad/glad.h"
#include "util.h"


namespace Acg {

	Texture::Texture(uint32_t width, uint32_t height, uint32_t m_component)
		:m_Width(width), m_Height(height), m_components(m_component)
	{
		Setfomat(m_component);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, width, height, 0, m_format, GL_UNSIGNED_BYTE, nullptr);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	Texture::Texture(const std::string& path, int m_components)
		:m_Path(path), m_components(m_components)
	{
		int width, height, n_component;
		stbi_set_flip_vertically_on_load(1);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &n_component, 0);
		if (data)
		{
			m_IsLoaded = true;
			m_Width = width;
			m_Height = height;
			m_components = m_components == -1 ? n_component : m_components;
			Setfomat(m_components);
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, width, height, 0, m_format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);

		}
		else
		{
			m_IsLoaded = false;
			std::cout << "Failed to load texture: " << path << std::endl;
		}
		stbi_image_free(data);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	const std::string Texture::GetFileName() const
	{
		auto filename = file::normalise(m_Path);
		return file::file_stem(filename);
	}

	void Texture::SetData(void* data, uint32_t size)
	{
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalformat, m_Width, m_Width, 0, m_format, GL_UNSIGNED_BYTE, data);
	}

	void Texture::SetFilter(uint32_t filter, uint32_t address)
	{
		Bind();
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, address);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, address);
	}

	void Texture::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	glm::vec4 Texture::Sample(glm::vec2 uv) const
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
			//AcgERROR("bad data!!!");
			return {};
		}
	}

	TextureHDR::TextureHDR(const std::string& path)
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

			LOG_ASSERT(m_format & m_internalformat, "Format not supported!");

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, m_data);

			LOG_INFO("TextureHDR load successfully")
		}
		else {
			LOG_ERROR("load TextureHDR failed, bad data!!")
		}

	}
	TextureHDR::TextureHDR(const std::vector<std::string>& path)
	{
		int width, height, channels;
		float* data = nullptr;
		{
			stbi_ldr_to_hdr_gamma(1.0f);
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
			const int roughnesses = 8;
			for (int i = 1; i < roughnesses; i++)
			{
				data = stbi_loadf(path[i].c_str(), &width, &height, &channels, 3);
				glTexImage2D(GL_TEXTURE_2D, i, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
			}
			LOG_INFO("TextureHDR with mipmap load successfully")
		}
		else {
			LOG_ERROR("load TextureHDR with mipmap failed, bad data!!")
		}
	}
	TextureHDR::~TextureHDR()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	void TextureHDR::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
	void TextureHDR::SaveHdrTexture(const std::string& path, uint32_t texture_id)
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

	glm::vec3 TextureHDR::Sample(const glm::vec2& uv) const
	{
		
		if (m_data)
		{
			int x = static_cast<int>(uv.x*(float)m_Width);
			int y = static_cast<int>(uv.y*(float)m_Height);
			float* d = (float*)(m_data + (y * m_Width + x)* 3);
			return glm::vec3(d[0], d[1],d[2]);
		}
		return {};
	}
	std::vector<float> TextureHDR::GetImgData()
	{
		std::vector<float> img;
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		GLint lwidth, lheight;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &lwidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &lheight);

		const int n_channels = 3;
		img.resize(lwidth * lheight * n_channels);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, img.data());

		return img;
	}
}