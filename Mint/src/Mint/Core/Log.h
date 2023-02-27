#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

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



#define MT_TRACE(...)		::Mint::Log::GetLogger()->trace(__VA_ARGS__);
#define MT_INFO(...)		::Mint::Log::GetLogger()->info(__VA_ARGS__);
#define MT_WARN(...)		::Mint::Log::GetLogger()->warn(__VA_ARGS__);
#define MT_ERROR(...)		::Mint::Log::GetLogger()->error(__VA_ARGS__);


