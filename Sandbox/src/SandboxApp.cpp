#include<Mint.h>
#include "Mint/Core/EntryPoint.h"
#include "SandboxLayerEg.h"


class SandBox : public Mint::Application
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

Mint::Application* Mint::CreateApplication()
{
	return new SandBox();
}