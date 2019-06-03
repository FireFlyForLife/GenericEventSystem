#pragma once
#include <cstdint>

namespace cof
{
	using TemplateTypeId = uint32_t;

	extern TemplateTypeId _templateIdCounter;

	template<typename T>
	TemplateTypeId GetIdFromType()
	{
		static TemplateTypeId myId = ++_templateIdCounter;

		return myId;
	}
}
