#pragma once
#include "Mint/Render/Framebuffer.h"

MT_NAMESPACE_BEGIN

class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(const FBSpecification& spec);
	virtual ~OpenGLFramebuffer();

	void Invalidate();

	virtual void Bind() override;
	virtual void Unbind() override;

	virtual void Resize(uint32_t width, uint32_t height) override;
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

	virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

	virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { MT_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

	virtual const FBSpecification& GetSpecification() const override { return m_Specification; }
private:
	uint32_t m_RendererID = 0;
	FBSpecification m_Specification;

	std::vector<FBTexture> m_ColorAttachmentSpecifications;
	FBTexture m_DepthAttachmentSpecification = FBTextureFormat::None;

	std::vector<uint32_t> m_ColorAttachments;
	uint32_t m_DepthAttachment = 0;
};





MT_NAMESPACE_END