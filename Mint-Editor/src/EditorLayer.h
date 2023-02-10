#pragma once
#include<Mint.h>


using namespace Mint;
class EditorLayer : public Mint::Layer
{

	Ref<VertexArray> va;
	Ref<Shader> shader_t;
	EditorCamera camera;
	Model* model = nullptr;
	MusicBuffer music;
	int sound = -1;
	SoundEffect soundplayer;
	Ref<Framebuffer> fbo;
	glm::vec2 viewportSize = { 0.0f, 0.0f };
	bool ishoved, isfocused;
	Ref<Texture2D> texture;

public:
	EditorLayer();
	~EditorLayer() =default;

	void OnUpdate(Timestep ts) override;

	void OnImGuiRender() override;

	void OnEvent(Mint::Event& event) override;
};
