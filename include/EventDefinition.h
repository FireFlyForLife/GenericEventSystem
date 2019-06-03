#pragma once
#include <TMPUtils.h>

namespace cof {
	struct EventTag{};

	template<typename... Ts>
	struct EventDef : public EventTag
	{
		using TArgs = TemplateParameterPack<Ts...>;
	};
}