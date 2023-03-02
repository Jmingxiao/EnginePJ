#pragma once
#include "Mint/Render/Texture.h"
#include "glad/glad.h"


static const uint32_t gl_texture_format_internal[] =
{
   GL_RED,
   GL_RGB8,
   GL_RGBA8,
};

static const uint32_t gl_texture_format[] =
{
   GL_RED,
   GL_RGB,
   GL_RGBA,
};

static const uint32_t gl_sampler_filter[] =
{
   GL_NEAREST,
   GL_LINEAR,
   GL_NEAREST_MIPMAP_NEAREST,
   GL_NEAREST_MIPMAP_LINEAR,
   GL_LINEAR_MIPMAP_NEAREST,
   GL_LINEAR_MIPMAP_LINEAR,
};

static const uint32_t gl_sampler_address[] =
{
   GL_CLAMP_TO_EDGE,
   GL_REPEAT,
   GL_MIRRORED_REPEAT
};

namespace Mint {
	
	class OpenGLTextureHDR : public TextureHDR
	{
	public:
		OpenGLTextureHDR(const std::string& path);
		OpenGLTextureHDR(const std::vector<std::string>& path);
		virtual ~OpenGLTextureHDR();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const HDR& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}
		static void SaveHdrTexture(const std::string& path, uint32_t texture_id);

	private:

		bool m_IsLoaded = false;
		float* m_data{ nullptr };
		std::string m_Path{};
		uint32_t m_Width{}, m_Height{};
		uint32_t m_RendererID{};
		uint32_t m_format{}, m_internalformat{}, m_components{};
	};

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t m_components);
		OpenGLTexture2D(const std::string& path, int m_components =-1);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual const std::string& GetPath() const override { return m_Path; }
		virtual const std::string GetFileName() const override;

		virtual void SetData(void* data, uint32_t size) override;

		virtual void SetFilter(uint32_t filter, uint32_t address) override;

		virtual void Bind(uint32_t slot = 0) const override;

		glm::vec4 Sample(glm::vec2 uv) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}

	private:
		void Setfomat(uint32_t n_component) override
		{
			if (n_component == 1)
			{
				m_format = GL_RED;
				m_internalformat = GL_RED;
			}
			else if (n_component == 3)
			{
				m_format = GL_RGB;
				m_internalformat = GL_RGB8;
			}
			else if (n_component == 4)
			{
				m_format = GL_RGBA;
				m_internalformat = GL_RGBA8;
			}
			else {
				MT_ERROR("texture format doesnot exist !!!");
			}
		}
	private:

		bool m_IsLoaded = false;
		uint8_t* m_data{nullptr};
		std::string m_Path{};
		uint32_t m_Width{}, m_Height{};
		uint32_t m_RendererID{};
		uint32_t m_format{}, m_internalformat{}, m_components{};
	};

}