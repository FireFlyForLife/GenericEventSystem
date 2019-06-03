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

//namespace OwnStd
//{
//	template<typename F, typename Tuple, size_t... I>
//	decltype(auto) ForwardCall(Tuple&& t, F&& f, std::index_sequence<I...>)
//	{
//		return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
//	}
//
//	template<typename F, typename Tuple>
//	decltype(auto) Invoke(Tuple&& t, F&& f)
//	{
//		constexpr size_t tSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
//		return ForwardCall(std::forward<Tuple>(t), std::forward<F>(f), std::make_index_sequence<tSize>());
//	}
//}


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

void PrintThreeFloats(float f1, float f2, float f3)
{
	std::cout << "3Floats: " << f1 << " " << f2 << " " << f3 << "\n";
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

struct StructWithNum
{
	uint32_t m_num;
};

struct StructWithNumHandler
{
	void PrintCallback(StructWithNum swn, float f)
	{
		std::cout << swn.m_num << " and: " << f << "\n";
	}
};

template<typename... TArgs>
void Test(EventDef<TArgs...> test) {
	(void)test;


}

int main()  // NOLINT(bugprone-exception-escape)
{
	InvokerBase* threeFloatsBase = new CallableInvoker<decltype(&PrintThreeFloats), float, float, float>(&PrintThreeFloats);
	threeFloatsBase->Run({}, 100.0f, 150.65f, 1200.12f);

	InvokerBase* invokerBase = new CallableInvoker<decltype(&TestFuncTest), int, double>(&TestFuncTest);
	invokerBase->Run({}, 15, 25.5);

	FunctionStruct testStruct{};
	InvokerBase* memberInvokerBase = new MemberFunctionInvoker<FunctionStruct, void(FunctionStruct::*)(int, double, char), int, double, char>(&testStruct, &FunctionStruct::Test);
	memberInvokerBase->Run({}, 7, 94.5, 'c');


	using TestEvent = EventDef<StructWithNum, float>;

	GenericEventSystem eventSystem{};

	//auto funcHandle = eventSystem.Register<decltype(&TestFuncTest), int, double>(&TestFuncTest);
	StructWithNumHandler swnHandler{};
	auto memFuncHandle = eventSystem.RegisterMemberFunction<TestEvent>(&swnHandler, &StructWithNumHandler::PrintCallback);

	eventSystem.FireEvent<TestEvent>(StructWithNum{65}, 100.50f);

	//EmptyStruct s;

	//eventSystem.Unregister<decltype(&TestFuncTest)>(funcHandle->Id());
	//eventSystem.Unregister<decltype(&FunctionStruct::Test)>(memFuncHandle->Id());

	//auto funcHandle2 = eventSystem.Register<decltype(&TestFuncTest), int, double>(&TestFuncTest);
	//eventSystem.Unregister(funcHandle2->Id());

	std::cout << "All tests ran!\n";
}
