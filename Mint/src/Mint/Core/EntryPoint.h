#pragma once

#include "Mint/Core/Base.h"
#include "Mint/Core/Application.h"

#ifdef MT_PLATFORM_WINDOWS

extern Mint::Application* Mint::CreateApplication();

int main(int argc, char** argv) {

	MT_WARN("Intialized Log");
	int a = 5;
	MT_INFO("Hello!	Var={0}",a);


	auto app = Mint::CreateApplication();
	app ->Run();
	delete app;

}

#endif // MT_PLATFORM_WINDOWS
