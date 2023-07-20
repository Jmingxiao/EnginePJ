#include "pch.h"
#include "FrameBuffer.h"
#include "glad/glad.h"

namespace Acg
{
    static const uint32_t s_MaxFramebufferSize = 8192;

	static GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
	{
		glCreateTextures(TextureTarget(multisampled), count, outID);
	}

	static void BindTexture(bool multisampled, uint32_t id)
	{
		glBindTexture(TextureTarget(multisampled), id);
	}

	static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
	}

	static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
	}

	static bool IsDepthFormat(FBTextureFormat format)
	{
		switch (format)
		{
			case FBTextureFormat::DEPTH24STENCIL8:  return true;
			case FBTextureFormat::Depth: return true;
		}

		return false;
	}

	static GLenum FBTextureFormatToGL(FBTextureFormat format)
	{
		switch (format)
		{
		case FBTextureFormat::RGBA8:       return GL_RGBA8;
		case FBTextureFormat::RGBA16:      return GL_RGBA16F;
		case FBTextureFormat::RGBA32:	   return GL_RGBA32F;
		case FBTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
		}

		LOG_ASSERT(false);
		return 0;
	}


    FrameBuffer::FrameBuffer(const FBSpecification& spec)
		: m_Specification(spec)
    {
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}
		Invalidate();
    }

    FrameBuffer::~FrameBuffer()
    {
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures((int32_t)m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
    }

    void FrameBuffer::Invalidate()
    {
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures((int32_t)m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			CreateTextures(multisample, m_ColorAttachments.data(), (int32_t)m_ColorAttachments.size());

			for (int32_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				BindTexture(multisample, m_ColorAttachments[i]);
				auto textureFormat = FBTextureFormatToGL(m_ColorAttachmentSpecifications[i].TextureFormat);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FBTextureFormat::RGBA8:
				case FBTextureFormat::RGBA16:
				case FBTextureFormat::RGBA32:
				{
					AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, textureFormat, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				}
				case FBTextureFormat::RED_INTEGER:
					AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FBTextureFormat::None)
		{
			CreateTextures(multisample, &m_DepthAttachment, 1);
			BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FBTextureFormat::Depth:
				AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH_COMPONENT32, GL_DEPTH_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			case FBTextureFormat::DEPTH24STENCIL8:
				AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			LOG_ASSERT(m_ColorAttachments.size() <= 5);
			GLenum buffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
			glDrawBuffers((int32_t)m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		LOG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Bind()
    {
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void FrameBuffer::Unbind()
    {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Blit(const Ref<FrameBuffer>& other)
    {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, other->GetID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height, 0, 0,
			m_Specification.Width, m_Specification.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    void FrameBuffer::Resize(uint32_t width, uint32_t height)
    {
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			LOG_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
    }

    int FrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
		LOG_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
    }

    void FrameBuffer::BindColorTexture(uint32_t slot, uint32_t index)
    {
		LOG_ASSERT(index < m_ColorAttachments.size());
		glBindTextureUnit(slot, m_ColorAttachments[index]);
    }

    void FrameBuffer::BindDepthTexture(uint32_t slot)
    {
		glBindTextureUnit(slot, m_DepthAttachment);
    }

    void FrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {
		LOG_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			FBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
    }
}
