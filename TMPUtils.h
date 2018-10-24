#pragma once
/*
 * Template Meta Programming Utilities
 * Includes things like, compile time loops.
 * And a parameter pack container
 */

template <int First, int Last>
struct static_for
{
	template <typename Fn>
	void operator()(Fn const& fn) const
	{
		if (First < Last) {
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
	{ }
};


template<typename... Ts>
struct TemplateParameterPack {};


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