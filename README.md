# GenericEventSystem

A Event system which holds all the event handlers in one place.

## Code example
```cpp
#include <GenericEventSystem.h>

#include <iostream>
#include <cstdarg>
#include <tuple>
#include <utility>

using namespace cof;


void TestFuncTest(int i, double d)
{
	std::cout << "Test Function! " << i << " " << d << '\n';
}

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

int main()
{
	using TestEvent = EventDef<StructWithNum, float>;
	using FailingEvent = EventDef<float, float, bool>;
	using IntDoubleEvent = EventDef<int, double>;

	GenericEventSystem eventSystem{};

	StructWithNumHandler swnHandler{};
	{
		auto memFuncHandle = eventSystem.RegisterMemberFunction<TestEvent>(&swnHandler, &StructWithNumHandler::PrintCallback);

		auto freeFuncHandle = eventSystem.Register<TestEvent>(&StructWithNumHandler::FreePrintCallback);
		//eventSystem.Register<FailingEvent>(&StructWithNumHandler::FreePrintCallback); //<-- This line will static_assert because the handler does not have the correct function signature

		eventSystem.FireEvent<TestEvent>(StructWithNum{ 65 }, 100.50f);
	}
	// func handles will automatically unregister themselves. thus making the next event not handled by any callback:
	eventSystem.FireEvent<TestEvent>(StructWithNum{ 45 }, 80.27f);

	// callbacks can also be manually unregistered by calling `Unregister` like this:
	auto funcHandle2 = eventSystem.Register<IntDoubleEvent>(&TestFuncTest);
	eventSystem.FireEvent<IntDoubleEvent>(20, 20.1);
	eventSystem.Unregister(funcHandle2->Id());

	std::cout << "All tests ran!\n";
}

```

## Design principles
This project was designed with ease of use and least complexity in mind. The meta template code is kept to a minumum and value semantics are used in favor of compile time recursion to aid in maintainability.

## Instalation
Build the project using visual studio, lead your additional includes to the `include/` directory and link against the build lib file.
