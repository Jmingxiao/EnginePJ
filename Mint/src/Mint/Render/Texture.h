#pragma once
#include "Mint/Core/Base.h"
#include "Mint/Util/FileSystem.h"

MT_NAMESPACE_BEGIN

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
	static Ref<Texture2D> Create(const std::string& path, uint32_t m_components);
};




MT_NAMESPACE_END