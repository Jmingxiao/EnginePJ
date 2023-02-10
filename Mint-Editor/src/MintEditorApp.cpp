#include<Mint.h>
#include "Mint/Core/EntryPoint.h"
#include "EditorLayer.h"

MT_NAMESPACE_BEGIN
class EditorApp : public Application
{

public:
	EditorApp()
	{
		PushLayer(new EditorLayer());
	}

	~EditorApp()
	{
	}

};

Application* CreateApplication()
{
	return new EditorApp();
}

MT_NAMESPACE_END