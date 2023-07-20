#pragma once
#include "Common/Common.h"
#include "Camera.h"
#include "Scene.h"

using Acg::Ref;


class ExampleLayer:public Acg::Layer {
	
	enum class DebugMode {
		DEBUG_CSM=0,DEBUG_SH,DEBUG_SSR,NONE
	};


	Acg::Model* model = nullptr;
	Ref<Acg::Shader> simple;
	Ref<Acg::Shader> m_shader;
	Ref<Acg::Shader> background;
	Ref<Acg::Shader> debugcsm;

	//sh test
	std::vector<glm::vec3>coefs;
	Ref<Acg::Shader> debugSh;

	//ssr test
	Ref<Acg::FrameBuffer> gbuffer;
	Ref<Acg::FrameBuffer> ssrColor;
	Ref<Acg::FrameBuffer> ssrBlurFB;


	Ref<Acg::Shader> gbufferInput;	
	Ref<Acg::Shader> ssrOutput;
	Ref<Acg::Shader> copyTexShader;
	Ref<Acg::Shader> ssrblur;


	Ref<Acg::Shader> Mixture;


	//ssao test


	Ref<Acg::TextureHDR> backgroundtexture;
	Ref<Acg::VertexArray> va;
	Ref<Acg::Camera> camera;

	Acg::Scene m_defaultScene;
	Acg::Scene m_scene;
	Acg::Scene testSH_scene;

	bool ssr_on = false;
	bool ssrblur_on = false;
	bool ssao_on = false;

	DebugMode debugmode = DebugMode::NONE;

private:
	int depthLayer{0};
	int frameIndex = 0;

public:
	ExampleLayer();
	~ExampleLayer() = default;

	void OnUpdate(float dt) override;

	void OnImGuiRender() override;

	void OnEvent(Acg::Event& event) override;

	void DrawSkybox();
	void DrawCSMDebug();
	void DrawSHDebug();
	void DrawssrOutput();

	void UpdatessrOutput();
	void UpdateGbuffer();
	void ShaderInit();
	void FramebufferInit();

};