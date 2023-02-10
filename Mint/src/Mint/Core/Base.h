#pragma once
#include <memory>

#ifdef MT_PLATFORM_WINDOWS
#if MT_DYNAMIC_LINK
	#ifdef MT_BUILD_DLL
		#define  __declspec(dllexport)
	#else
		#define  __declspec(dllimport)
	#endif
#else
	#define  MINT_API
#endif
#else
	#error Mint only support Windows!
#endif // MT_PLATFORM_WINDOWS

#ifdef MT_ENABLE_ASSERTS
	#define MT_ASSERT(x,...) {if(!(x)){ MT_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); }}
	#define MT_ASSERT(x,...) {if(!(x)){ MT_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); }}
#else
	#define MT_ASSERT(x,...)
	#define MT_ASSERT(x,...)
#endif // MT_ENABLE_ASSERTS

#define BIT(x) (1 << x)

#define MT_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }


#ifndef MT_NAMESPACE_BEGIN
#define MT_NAMESPACE_BEGIN \
	namespace Mint {
#endif // !SAND_TABLE_NAMESPACE_BEGIN

#ifndef MT_NAMESPACE_END
#define MT_NAMESPACE_END \
	}
#endif



MT_NAMESPACE_BEGIN

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
MT_NAMESPACE_END



#include "Mint/Core/Log.h"