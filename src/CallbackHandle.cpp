#include "CallbackHandle.h"
#include "GenericEventSystem.h"

namespace cof
{

	CallbackHandle::~CallbackHandle()
	{
		if (m_eventSystem) {
			m_eventSystem->Unregister(m_id);
		}
	}

	CallbackHandle::CallbackHandle(CallbackHandle&& other) noexcept : m_id(other.m_id),
		m_eventSystem(other.m_eventSystem)
	{}

	CallbackHandle& CallbackHandle::operator=(CallbackHandle&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_id = other.m_id; other.m_id = 0;
		m_eventSystem = other.m_eventSystem; other.m_eventSystem = nullptr;
		return *this;
	}
}
