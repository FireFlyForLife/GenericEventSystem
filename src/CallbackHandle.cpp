#include "CallbackHandle.h"
#include "GenericEventSystem.h"

namespace cof
{
	CallbackHandle::~CallbackHandle()
	{
		if (eventSystem && eventTypeId != 0 && id != 0) {
			eventSystem->Unregister(eventTypeId, id);
		}

		eventSystem = nullptr;
		eventTypeId = 0;
		id = 0;
	}

	CallbackHandle::CallbackHandle(CallbackHandle&& other) noexcept : id(other.id),
		eventTypeId(other.eventTypeId),
		eventSystem(other.eventSystem)
	{
		other.id = 0;
		other.eventTypeId = 0;
		other.eventSystem = nullptr;
	}

	CallbackHandle& CallbackHandle::operator=(CallbackHandle&& other) noexcept
	{
		if (this == &other)
			return *this;

		id = other.id; other.id = 0;
		eventTypeId = other.eventTypeId; other.eventTypeId = 0;
		eventSystem = other.eventSystem; other.eventSystem = nullptr;

		return *this;
	}
}
