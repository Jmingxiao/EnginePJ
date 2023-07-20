#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "Base.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Acg {
	class Log
	{

	public:
		static Ref<spdlog::logger> GetLogger();

	private:
		static Ref<spdlog::logger> s_Logger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

#define LOG_TRACE(...)		::Acg::Log::GetLogger()->trace(__VA_ARGS__);
#define LOG_INFO(...)		::Acg::Log::GetLogger()->info(__VA_ARGS__);
#define LOG_WARN(...)		::Acg::Log::GetLogger()->warn(__VA_ARGS__);
#define LOG_ERROR(...)		::Acg::Log::GetLogger()->error(__VA_ARGS__);

// Path: ACG_Project\src\Common\Log.cpp