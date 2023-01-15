#include<Hazel.h>

class ExampleLayer : public Hazel::Layer
{
public :
	ExampleLayer()
		:Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB))
			HZ_TRACE("TAB key is pressed");
	}

	void OnEvent(Hazel::Event& event) override
	{
		//HZ_TRACE("{0}", event);
	}
};



class SandBox : public Hazel::Application 
{

public:
	SandBox() 
	{
		PushLayer(new ExampleLayer());
		PushLayer(new Hazel::ImGuiLayer());
	}

	~SandBox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new SandBox();
}