#pragma once
/*
 * Template Meta Programming Utilities
 * Includes things like, compile time loops.
 * And a parameter pack container
 */

namespace cof
{
	struct EmptyStruct {};

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

}