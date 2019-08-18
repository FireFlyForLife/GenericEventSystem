#pragma once
/*
 * Template Meta Programming Utilities
 * Includes things like, compile time loops.
 * And a parameter pack container
 */

#include <type_traits>
#if _MSVC_LANG < 201703L || defined(DISABLE_CPP_17)
#include <functional>
#include <invoke.hpp>
#endif


namespace cof
{
	struct EmptyStruct {};

#if _MSVC_LANG >= 201703L
	template <int First, int Last>
	struct static_for
	{
		template <typename Fn>
		void operator()(Fn const& fn) const
		{
			if constexpr (First < Last) {
				fn(std::integral_constant<int, First>{});
				static_for<First + 1, Last>()(fn);
			}
		}
	};
	template <int N>
	struct static_for<N, N>
	{
		template <typename Fn>
		void operator()(Fn const& fn) const
		{
			(void)fn;
		}
	};
#endif

	template<typename T, typename... Ts>
	struct TemplateParameterPack
	{
		using Type = T;
		using Others = TemplateParameterPack<Ts...>;

		constexpr static bool IsLast = false;
	};

	template<typename T>
	struct TemplateParameterPack<T>
	{
		using Type = T;

		constexpr static bool IsLast = true;
	};



	template<typename TClass, typename TRet, typename... TArgs>
	struct PointerToMemberFunctionTypes
	{
		using Class = TClass;
		using ReturnType = TRet;
		using Arguments = TemplateParameterPack<TArgs...>;
	};

	template<typename T>
	void StaticAssertPrint()
	{
#ifdef _MSC_VER
		static_assert(false, __FUNCSIG__);
#endif
	}

	template<typename... Ts>
	struct TypeArray {};
	static_assert(std::is_trivial<TypeArray<int, double&>>::value, "TypeArray should always be trivial");

#if _MSVC_LANG >= 201703L && !defined(DISABLE_CPP_17)
	template<typename T, typename... Ts>
	using IsInvocable = std::is_invocable<T, Ts...>;

	template<typename T, typename... Ts>
	constexpr bool IsInvocable_v = std::is_invocable_v<T, Ts...>;
#else
	template <typename F, typename... Args>
	struct IsInvocable :
		std::is_constructible<
			std::function<void(Args ...)>,
			std::reference_wrapper<typename std::remove_reference<F>::type>
		>
	{ };

	template<typename T, typename... Ts>
	constexpr bool IsInvocable_v = IsInvocable<T, Ts...>::value;
#endif

#if _MSVC_LANG >= 201703L
	//! An alias of std::conjunction. With a C++14 fallback implementation
	//! @see std::conjunction documentation
	template<typename... B>
	using Conjunction = std::conjunction<B...>;

	//! An alias of std::conjunction_v. With a C++14 fallback implementation
	//! @see std::conjunction_v documentation
	template<typename... B>
	inline constexpr bool Conjunction_v = std::conjunction_v<B...>;
#else
	//! An alias of std::conjunction. This is the C++14 fallback implementation
	//! @see std::conjunction documentation
	template<class...> struct Conjunction : std::true_type { };
	template<class B1> struct Conjunction<B1> : B1 { };
	template<class B1, class... Bn>
	struct Conjunction<B1, Bn...>
		: std::conditional_t<bool(B1::value), Conjunction<Bn...>, B1> {};

	//! An alias of std::conjunction_v. This is the C++14 fallback implementation
	//! @see std::conjunction_v documentation
	template<class... B>
	constexpr bool Conjunction_v = Conjunction<B...>::value;
#endif

	template<typename TCallable, typename TTuple>
	constexpr decltype(auto) Apply(TCallable&& callable, TTuple&& tupleArgs) {
#if _MSVC_LANG >= 201703L
		return std::apply(std::forward<TCallable>(callable), std::forward<TTuple>(tupleArgs));
#else
		return invoke_hpp::apply(std::forward<TCallable>(callable), std::forward<TTuple>(tupleArgs));
#endif
	}
}