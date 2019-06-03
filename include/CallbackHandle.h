#pragma once
#include <cstdint>

namespace cof
{
	class GenericEventSystem;

	class CallbackHandle
	{
	public:
		using CallbackHandleId = uint32_t;

	private:
		CallbackHandleId m_id;
		GenericEventSystem* m_eventSystem;

	public:
		CallbackHandle() : m_id(0), m_eventSystem(nullptr) 
		{}
		CallbackHandle(CallbackHandleId id, GenericEventSystem* eventSystem) : m_id(id), m_eventSystem(eventSystem)
		{}
		~CallbackHandle();

		CallbackHandle(const CallbackHandle& other) = delete;
		CallbackHandle(CallbackHandle&& other) noexcept;
		CallbackHandle& operator=(const CallbackHandle& other) = delete;
		CallbackHandle& operator=(CallbackHandle&& other) noexcept;

		CallbackHandleId Id() const { return m_id; }
		bool IsValid() const { return m_id != 0 && m_eventSystem != nullptr; }
	};
}
