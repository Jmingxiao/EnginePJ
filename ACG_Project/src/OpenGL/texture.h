#pragma once
#include "glm/glm.hpp"
#include "glad/glad.h"
namespace Acg {

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
	class Texture {

	public:
		Texture(uint32_t width, uint32_t height, uint32_t m_components);
		Texture(const std::string& path, int m_components = -1);
		~Texture();

		uint32_t GetWidth() const  { return m_Width; }
		uint32_t GetHeight() const  { return m_Height; }
		uint32_t GetRendererID() const  { return m_RendererID; }
		const std::string& GetPath() const  { return m_Path; }
		const std::string GetFileName() const ;
		void SetData(void* data, uint32_t size) ;
		void SetFilter(uint32_t filter, uint32_t address) ;
		void Bind(uint32_t slot = 0) const ;

		glm::vec4 Sample(glm::vec2 uv) const ;

		bool IsLoaded() const { return m_IsLoaded; }

		bool operator==(const Texture& other) const 
		{
			return m_RendererID == other.GetRendererID();
		}

	private:
		void Setfomat(uint32_t n_component) 
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
				std::cout<<"texture format doesnot exist !!!";
			}
		}
	private:

		bool m_IsLoaded = false;
		uint8_t* m_data{ nullptr };
		std::string m_Path{};
		uint32_t m_Width{}, m_Height{};
		uint32_t m_RendererID{};
		uint32_t m_format{}, m_internalformat{}, m_components{};
	};

	class TextureHDR
	{
	public:
		TextureHDR(const std::string& path);
		TextureHDR(const std::vector<std::string>& path);
		~TextureHDR();

		uint32_t GetWidth() const  { return m_Width; }
		uint32_t GetHeight() const  { return m_Height; }
		uint32_t GetRendererID() const  { return m_RendererID; }
		void Bind(uint32_t slot = 0) const ;
		bool IsLoaded() const  { return m_IsLoaded; }
		bool operator==(const TextureHDR& other) const
		{
			return m_RendererID == other.GetRendererID();
		}
		static void SaveHdrTexture(const std::string& path, uint32_t texture_id);
		glm::vec3 Sample(const glm::vec2& uv) const;
		std::vector<float> GetImgData();
	private:

		bool m_IsLoaded = false;
		float* m_data{ nullptr };
		std::string m_Path{};
		uint32_t m_Width{}, m_Height{};
		uint32_t m_RendererID{};
		uint32_t m_format{}, m_internalformat{}, m_components{};


	};

}