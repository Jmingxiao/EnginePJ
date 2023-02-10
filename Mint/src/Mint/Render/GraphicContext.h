#pragma once

namespace Mint {

	class GraphicContext
	{
	public:
		virtual	void Init() = 0;
		virtual void SwapBuffers() =0;

	};


}