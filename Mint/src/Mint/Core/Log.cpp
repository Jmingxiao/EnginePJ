#include "pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/sinks/basic_file_sink.h>


namespace Mint{

	std::shared_ptr<spdlog::logger> Log::s_Logger =nullptr;

	Ref<spdlog::logger> Log::GetLogger()
	{
		if (!s_Logger)
		{
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("Mint.log", true));

			logSinks[0]->set_pattern("%^[%T] %n: %v%$");
			logSinks[1]->set_pattern("[%T] [%l] %n: %v");

			s_Logger = CreateRef<spdlog::logger>("Mint", begin(logSinks), end(logSinks));
			spdlog::register_logger(s_Logger);
			s_Logger->set_level(spdlog::level::trace);
			s_Logger->flush_on(spdlog::level::trace);
		}
		return s_Logger;

	}

}