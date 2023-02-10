#pragma once


#include "Mint/Core/Base.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)


namespace Mint
{
	class Log
	{

	public:
		static Ref<spdlog::logger> GetLogger();

	private:
		static Ref<spdlog::logger> s_Logger;
	};
}


#define MT_TRACE(...)		::Mint::Log::GetLogger()->trace(__VA_ARGS__);
#define MT_INFO(...)		::Mint::Log::GetLogger()->info(__VA_ARGS__);
#define MT_WARN(...)		::Mint::Log::GetLogger()->warn(__VA_ARGS__);
#define MT_ERROR(...)		::Mint::Log::GetLogger()->error(__VA_ARGS__);


