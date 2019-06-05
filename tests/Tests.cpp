#include "catch.hpp"

#include <GenericEventSystem.h>

#include <iostream>
#include <cstdarg>
#include <tuple>
#include <utility>


using namespace cof;


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

	static void FreePrintCallback(StructWithNum swn, float f)
	{
		std::cout << "Free function callback: " << swn.m_num << " and: " << f << "\n";
	}
};


TEST_CASE("Reference lvalue test") 
{
	using FloatRefEvent = EventDef<float&>;

	GenericEventSystem eventSystem{};

	auto handle = eventSystem.RegisterListener<FloatRefEvent>([](float& f) { f = 200.0f; });

	float argument = 100.0f;
	//Before, argument is 100
	REQUIRE(std::abs(argument - 100.0f) < 0.1f);
	eventSystem.FireEvent<FloatRefEvent>(argument);
	//Afterwards, argument should be 200
	REQUIRE(std::abs(argument - 200.0f) < 0.1f);
}

TEST_CASE("Basic test")
{
	InvokerBase* threeFloatsBase = new CallableInvoker<decltype(&PrintThreeFloats), float, float, float>(&PrintThreeFloats);
	auto tArgs1 = std::make_tuple(100.0f, 150.65f, 1200.12f);
	threeFloatsBase->Run(&tArgs1);

	InvokerBase* invokerBase = new CallableInvoker<decltype(&TestFuncTest), int, double>(&TestFuncTest);
	auto tArgs2 = std::make_tuple(15, 25.5);
	invokerBase->Run(&tArgs2);

	FunctionStruct testStruct{};
	InvokerBase* memberInvokerBase = new MemberFunctionInvoker<FunctionStruct, void(FunctionStruct::*)(int, double, char), int, double, char>(&testStruct, &FunctionStruct::Test);
	auto tArgs3 = std::make_tuple(7, 94.5, 'c');
	memberInvokerBase->Run(&tArgs3);


	using TestEvent = EventDef<StructWithNum, float>;
	using FailingEvent = EventDef<float, float, bool>;
	using IntDoubleEvent = EventDef<int, double>;

	GenericEventSystem eventSystem{};

	//auto funcHandle = eventSystem.Register<decltype(&TestFuncTest), int, double>(&TestFuncTest);
	StructWithNumHandler swnHandler{};
	{
		auto memFuncHandle = eventSystem.RegisterListener<TestEvent>(&swnHandler, &StructWithNumHandler::PrintCallback);

		auto freeFuncHandle = eventSystem.RegisterListener<TestEvent>(&StructWithNumHandler::FreePrintCallback);
		//eventSystem.Register<FailingEvent>(&StructWithNumHandler::FreePrintCallback);

		eventSystem.FireEvent<TestEvent>(StructWithNum{ 65 }, 100.50f);
	}
	eventSystem.FireEvent<TestEvent>(StructWithNum{ 65 }, 100.50f);


	auto funcHandle2 = eventSystem.RegisterListener<IntDoubleEvent>(&TestFuncTest);
	eventSystem.FireEvent<IntDoubleEvent>(20, 20.1);
	eventSystem.Unregister(funcHandle2->Id());

	{
		auto genericMemberFunc = eventSystem.RegisterListener<TestEvent>(&swnHandler, &StructWithNumHandler::PrintCallback);
		auto genericFreeFunc = eventSystem.RegisterListener<TestEvent>(&StructWithNumHandler::FreePrintCallback);

		eventSystem.FireEvent<TestEvent>(StructWithNum{ 65 }, 100.50f);
	}
	eventSystem.FireEvent<TestEvent>(StructWithNum{ 65 }, 100.50f);

	std::cout << "All tests ran!\n";
}
