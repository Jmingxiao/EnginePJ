#pragma once
#include<Mint.h>

namespace Mint {

	class MsFramebuffer {
	public:
		void init();
		void blit();
		bool Resizeviewport( const glm::vec2& viewportsize);
		uint32_t GetviewportTextureID();
		int selectEntity(const glm::vec2&, const glm::vec2&);

	public:
		Ref<Framebuffer> fbo;
		Ref<Framebuffer> msfbo;
		Ref<Framebuffer> intermidefbo;
		Ref<Shader> copytextureShader;
	};
}