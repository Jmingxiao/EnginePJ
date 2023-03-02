#pragma once
#include "Mint/Core/Base.h"
#include "Mint/Util/FileSystem.h"
#include "glm/glm.hpp"

MT_NAMESPACE_BEGIN

class HDR {
public:
	virtual ~HDR() = default;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual uint32_t GetRendererID() const = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;
	virtual bool IsLoaded() const = 0;
	virtual bool operator==(const HDR& other) const = 0;
};



class Texture
{
public:
	virtual ~Texture() = default;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual uint32_t GetRendererID() const = 0;

	virtual const std::string& GetPath() const = 0;
	virtual const std::string GetFileName() const = 0;

	virtual void SetData(void* data, uint32_t size) = 0;
	virtual void Setfomat(uint32_t n_component) = 0;
	virtual void SetFilter(unsigned int filter,unsigned int address) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;
	virtual glm::vec4 Sample(glm::vec2 uv) const =0;
	virtual bool IsLoaded() const = 0;

	virtual bool operator==(const Texture& other) const = 0;
};

class Texture2D : public Texture
{
public:
	static Ref<Texture2D> Create(uint32_t width, uint32_t height, uint32_t m_components);
	static Ref<Texture2D> Create(const std::string& path, int m_components = -1);
};

class TextureHDR : public HDR
{
public:
	static Ref<TextureHDR> Create(const std::string& path);
	static Ref<TextureHDR> Create(const std::vector<std::string>& path);
	static void SaveHdrTexture(const std::string& path, uint32_t texture_id);
};

MT_NAMESPACE_END