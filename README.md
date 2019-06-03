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

	GenericEventSystem eventSystem{};

	StructWithNumHandler swnHandler{};
	auto memFuncHandle = eventSystem.RegisterMemberFunction<TestEvent>(&swnHandler, &StructWithNumHandler::PrintCallback);

	eventSystem.Register<TestEvent>(&StructWithNumHandler::FreePrintCallback);
	//eventSystem.Register<FailingEvent>(&StructWithNumHandler::FreePrintCallback); //<-- This line will static_assert because the handler does not have the correct function signature

	eventSystem.FireEvent<TestEvent>(StructWithNum{ 65 }, 100.50f);

	// This will soon be implemented
	//eventSystem.Unregister<decltype(&TestFuncTest)>(funcHandle->Id());
	//eventSystem.Unregister<decltype(&FunctionStruct::Test)>(memFuncHandle->Id());

	std::cout << "All tests ran!\n";
}

```

## Design principles
This project was designed with ease of use and least complexity in mind. The meta template code is kept to a minumum and value semantics are used in favor of compile time recursion to aid in maintainability.

## Instalation
Build the project using visual studio, lead your additional includes to the `include/` directory and link against the build lib file.
