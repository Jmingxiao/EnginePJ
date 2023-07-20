#include "pch.h"
#include "Common/Common.h"
#include "SandBox/ExampleLayer.h"
#include "Sampler/Harmonics.h"
#include "Sampler/SphereMap.h"


class SandBox : public Acg::Application
{

public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
	}

	~SandBox()
	{
	}
};

Acg::Application* Acg::CreateApplication()
{
	return new SandBox();
}


int main() {

	auto app = Acg::CreateApplication();
	app->Run();

	return 0;
}