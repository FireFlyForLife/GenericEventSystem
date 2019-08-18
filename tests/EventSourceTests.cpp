#include "catch.hpp"
#include "EventSource.h"

void FreeFunction(int i, double& d)
{
	
}

class SomeClass
{
public:
	void MemberFunction(int i, double& d)
	{
		
	}

	static void StaticMemberFunction(int i, double& d)
	{
		
	}
};

TEST_CASE("EventSource test")
{
	using namespace cof;

	EventSource<int, double&> eventSource{};

	SomeClass someClass{};

	eventSource.add([](int i, double& d) {});
	eventSource.add(FreeFunction);
	eventSource.add([&someClass](int i, double& d) { someClass.MemberFunction(i, d); });
	eventSource.add(SomeClass::StaticMemberFunction);

	double num = 0.4;
	eventSource.call(100, num);
}