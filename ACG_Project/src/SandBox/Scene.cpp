#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "OpenGL/model.h"
#include "OpenGL/Renderer.h"
#include "Sampler/Harmonics.h"
#include "Sampler/SphereMap.h"

void Acg::Scene::LoadScene(bool hasshadow,const std::string& path)
{
	if (hasshadow) { csm = CreateRef<CSM>(4096, Light()); };
	if (path == "") {
		envmappath = "assets/textures/envmaps/lovelySky/";
		background = CreateRef<TextureHDR>(envmappath+"001.hdr");
	}
	else { background = CreateRef<TextureHDR>(path + "001.hdr"); envmappath = path; }
}

void Acg::Scene::DrawScene(const Ref<Shader>& shader,const Ref<Camera>& cam)
{
	shader->Bind();
	shader->SetMat4("projMatrix", cam->GetProjection());
	shader->SetMat4("viewMatrix", cam->GetViewMatrix());
	shader->SetFloat("environment_multiplier", 1.5f);
	shader->SetFloat("lightIntensity", GetLight().intensity);
	auto lightdirvs = cam->GetViewMatrix() * glm::vec4(GetLight().direction,0.0f);
	shader->SetFloat3("lightDirection",lightdirvs);
	shader->SetFloat3("lightcolor", GetLight().color);
	int id = 0;
	for (auto& ent : entities) {
		shader->SetMat4("modelMatrix", ent.second.modelMatrix);
		shader->SetInt("objectID", id++);
		Acg::render(ent.second.model, shader);
	}
	
	// draw light
	{


	}
}

void Acg::Scene::DrawCascadeShadowMap(const Ref<Camera>& cam)
{
	if (csm.get()) {
		csm->Bind(cam);
		for (auto& ent : entities) {
			csm->depthShader->SetMat4("modelMatrix", ent.second.modelMatrix);
			Acg::render(ent.second.model, csm->depthShader);
		}
		csm->Unbind();
	}
}


void Acg::Scene::PrecomputeEnvmaps(bool resample)
{
	std::fstream fileStream;
	std::string filename = "001.hdr";
	std::string coefname = "lighting.txt";
	const std::string coefilenames[] = { envmappath + coefname };
	for (const auto& coefile : coefilenames) {
		fileStream.open(coefile);
		if (fileStream.fail()|| resample) {
			using namespace Sampler;
			using namespace std;
			SphericalMap spm(background);
			Harmonics harmonics(3);
			{
				const unsigned int samplenum = 100000;
				std::cout << "sampling ..." << std::endl;
				std::vector<Vertex> verticies = spm.RandomSample(samplenum);
				harmonics.Evaluate(verticies);
			}
			cout << "---------- coefficients ----------" << endl;
			auto coefs = harmonics.getCoefficients();
			string coefstr = CoefficientsString(coefs);
			cout << coefstr;
			WriteString2File(coefile, coefstr);
		}
	}

	loadReflTexture();

	if (!prefilteredBackground->IsLoaded() || resample) {

		int reflelevel = 8;
		std::vector<Ref<FrameBuffer>> fb(reflelevel);
		int wi = background->GetWidth(), hi = background->GetHeight();
		for (int i = 0; i < reflelevel; i++) {
			FBSpecification fbspec;
			fbspec.Attachments = { FBTextureFormat::RGBA32 };
			fbspec.Width = wi >> i;
			fbspec.Height = hi >> i;
			fb[i] = CreateRef<FrameBuffer>(fbspec);
		}
		std::vector<ShaderInfo> reflecInfo
		{
			{ShaderType::VETEX_SHADER,"assets/shaders/background.vert"},
			{ShaderType::FRAGMENT_SHADER,"assets/shaders/reflectivityMap.frag"}
		};
		Ref<Shader>refleshader = CreateRef<Shader>(reflecInfo, "refleshader");

		refleshader->Bind();
		for (int i = 0; i < fb.size(); i++) {
			fb[i]->Bind();
			RendererCommand::SetViewport(0, 0, fb[i]->GetSpecification().Width, fb[i]->GetSpecification().Height);
			RendererCommand::Clear();
			refleshader->SetFloat("a_roughness", (float)i + 0.5f);
			background->Bind();
			RendererCommand::DrawFullScreenQuad();
			fb[i]->Unbind();
		}
		for (int i = 0; i < fb.size(); i++) {
			TextureHDR::SaveHdrTexture(envmappath + "ref" + std::to_string(i), fb[i]->GetColorAttachmentRendererID());
		}
		
		loadReflTexture();
	}

}

void Acg::Scene::AddModel(const std::string path, const std::string& name, glm::mat4 modelmatrix)
{
	Model* model = loadModelFromOBJ(path);
	entities[name] = { modelmatrix,model };
}

void Acg::Scene::BindEnvmaps(int backgroundslot, int backgroundreflslot)
{
	background->Bind(backgroundslot);
	prefilteredBackground->Bind(backgroundreflslot);
}

void Acg::Scene::Bindbackground(int slot)
{
	background->Bind(slot);
}

void Acg::Scene::Bindrefletmap(int slot)
{
	prefilteredBackground->Bind(slot);
}

void Acg::Scene::loadReflTexture()
{
	std::vector<std::string> filenames;
	for (int i = 0; i < 8; i++)
		filenames.push_back(envmappath + "ref" + std::to_string(i) + ".hdr");
	
	prefilteredBackground = CreateRef<TextureHDR>(filenames);
}

