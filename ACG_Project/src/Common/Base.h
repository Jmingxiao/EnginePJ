#pragma once
#include <memory>

#ifdef ENABLE_ASSERTS
#define AcgASSERT(x,...) {if(!(x)){ AcgERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); }}
#define AcgASSERT(x,...) {if(!(x)){ AcgERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); }}
#else
#define LOG_ASSERT(x,...)
#define LOG_ASSERT(x,...)
#endif // AcgENABLE_ASSERTS

#define BIT(x) (1 << x)

#define ACG_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Acg {
	template<typename T>
	using Ptr = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ptr<T> CreatePtr(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}