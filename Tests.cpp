#include <GenericEventSystem.h>


#include <iostream>
#include <cstdarg>
#include <tuple>
#include <utility>

using namespace cof;

template<typename T, typename... Ts>
struct PrintTypes
{
	PrintTypes()
	{
		std::cout << typeid(T).name() << '\n';
		PrintTypes<Ts...>{};
	}
};

template<typename T>
struct PrintTypes<T>
{
	PrintTypes()
	{
		std::cout << typeid(T).name() << '\n';
	}
};

namespace OwnStd
{
	template<typename F, typename Tuple, size_t... I>
	decltype(auto) ForwardCall(Tuple&& t, F&& f, std::index_sequence<I...>)
	{
		return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
	}

	template<typename F, typename Tuple>
	decltype(auto) Invoke(Tuple&& t, F&& f)
	{
		constexpr size_t tSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
		return ForwardCall(std::forward<Tuple>(t), std::forward<F>(f), std::make_index_sequence<tSize>());
	}
}


template<typename TRet, typename... TArgs>
void TypesOfFunctionPtr(TRet(*)(TArgs...))
{
	std::cout << typeid(TRet).name() << '\n';

	PrintTypes<TArgs...>{};
}

template<typename TClass, typename TRet, typename... TArgs>
void TypesOfMemberFunctionPtr(TRet (TClass::*)(TArgs...))
{
	std::cout << typeid(TClass).name() << '\n';
	std::cout << typeid(TRet).name() << '\n';
	PrintTypes<TArgs...>{};
}

void TestFuncTest(int i, double d)
{
	std::cout << "Test Function! " << i << " " << d << '\n';
}

struct FunctionStruct
{
	void Test(int i , double d , char c)
	{
		std::cout << "Struct test function! " << i << " " << d << " " << c << '\n';
	}
};


int main()
{
	InvokerBase* invokerBase = new CallableInvoker<decltype(&TestFuncTest), int, double>(&TestFuncTest);
	invokerBase->Run({}, 15, 25.5);

	FunctionStruct testStruct{};
	InvokerBase* memberInvokerBase = new MemberFunctionInvoker<FunctionStruct, void(FunctionStruct::*)(int, double, char)>(testStruct, &FunctionStruct::Test);
	memberInvokerBase->Run({}, 7, 94.5, 'c');
}
