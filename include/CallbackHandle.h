#pragma once
#include <cstdint>
#include <TemplateTypeId.h>

namespace cof
{
	class GenericEventSystem;

	class CallbackHandle
	{
	public:
		using CallbackHandleId = uint32_t;

	private:
		CallbackHandleId id;
		TemplateTypeId eventTypeId;
		GenericEventSystem* eventSystem;

	public:
		CallbackHandle() 
			: id(0), eventTypeId(0), eventSystem(nullptr) 
		{}
		CallbackHandle(CallbackHandleId id, TemplateTypeId eventTypeId, GenericEventSystem* eventSystem) 
			: id(id), eventTypeId(eventTypeId), eventSystem(eventSystem)
		{}
		~CallbackHandle();

		CallbackHandle(const CallbackHandle& other) = delete;
		CallbackHandle(CallbackHandle&& other) noexcept;
		CallbackHandle& operator=(const CallbackHandle& other) = delete;
		CallbackHandle& operator=(CallbackHandle&& other) noexcept;

		CallbackHandleId Id() const { return id; }
		TemplateTypeId EventTypeId() const { return eventTypeId; }
		bool IsValid() const { return id != 0 && eventTypeId != 0 && eventSystem != nullptr; }
	};
}
