#pragma once

namespace Acg {
	enum class FBTextureFormat
	{
		None = 0,
		// Color
		RGBA8,
		RGBA16,
		RGBA32,
		RED_INTEGER,
		// Depth/stencil
		DEPTH24STENCIL8,
		// Defaults
		Depth
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



	class FrameBuffer {

	public:
		FrameBuffer(const FBSpecification& spec);
		~FrameBuffer();

		void Invalidate();

		void Bind() ;
		void Unbind() ;
		void Blit(const Ref<FrameBuffer>& other) ;
		void Resize(uint32_t width, uint32_t height) ;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) ;
		void BindColorTexture(uint32_t slot = 0, uint32_t index = 0) ;
		void BindDepthTexture(uint32_t slot = 0) ;
		void ClearAttachment(uint32_t attachmentIndex, int value) ;

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const  { LOG_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

		const FBSpecification& GetSpecification() const  { return m_Specification; }
		const uint32_t GetID() { return m_RendererID; }

	protected:
		uint32_t m_RendererID = 0;
		FBSpecification m_Specification;

		std::vector<FBTexture> m_ColorAttachmentSpecifications;
		FBTexture m_DepthAttachmentSpecification = FBTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;

	};




}