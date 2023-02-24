#pragma once

#include "Mint/Core/Base.h"


MT_NAMESPACE_BEGIN

enum class FBTextureFormat
{
	None = 0,
	// Color
	RGBA8,
	RGBA32,
	RED_INTEGER,
	// Depth/stencil
	DEPTH24STENCIL8,
	// Defaults
	Depth = DEPTH24STENCIL8
};

struct FBTexture
{
	FBTexture() = default;
	FBTexture(FBTextureFormat format)
		: TextureFormat(format) {}

	FBTextureFormat TextureFormat = FBTextureFormat::None;
	// TODO: filtering/wrap
};

struct FBAttachment
{
	FBAttachment() = default;
	FBAttachment(std::initializer_list<FBTexture> attachments)
		: Attachments(attachments) {}

	std::vector<FBTexture> Attachments;
};

struct FBSpecification
{
	uint32_t Width = 0, Height = 0;
	FBAttachment Attachments;
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class Framebuffer
{
public:
	virtual ~Framebuffer() = default;

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void Blit(const Ref<Framebuffer>& other) = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

	virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

	virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

	virtual const FBSpecification& GetSpecification() const = 0;
	virtual const uint32_t GetID() = 0;
	static Ref<Framebuffer> Create(const FBSpecification& spec);
};




MT_NAMESPACE_END