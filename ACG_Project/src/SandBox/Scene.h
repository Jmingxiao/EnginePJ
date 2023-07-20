#pragma once
#include "Common/Common.h"
#include "CascadeShadowMap.h"


namespace Acg {
	
	class Camera;

	struct Entity {
		glm::mat4 modelMatrix{ glm::mat4(1) };
		Model* model{ nullptr };
	};
	class Scene{
	public:
		void LoadScene(bool hasshadow, const std::string& backgroundpath="");

		void DrawScene(const Ref<Shader>& shader,const Ref<Camera>& cam);

		void DrawCascadeShadowMap( const Ref<Camera>& cam);

		void PrecomputeEnvmaps(bool resample);

		void AddModel(const std::string path, const std::string& name,glm::mat4 modelmatrix = glm::mat4(1.0f));

		void BindEnvmaps(int backslot = 6 , int backreflslot =7);

		void Bindbackground(int slot = 0);
		void Bindrefletmap(int slot = 0);
		Light& GetLight() { return csm->m_light; }
		void loadReflTexture();

	public:
		float envMultiplier{1.5};
		std::map<std::string, Entity> entities;
		Ref<CSM> csm;
		Ref<TextureHDR> background;
		Ref<TextureHDR> prefilteredBackground;
		std::string envmappath;
	};

}