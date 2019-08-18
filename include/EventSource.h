#pragma once
#include <vector>
#include <functional>
#include "light_flat_value_map.h"

namespace cof {
	/**
	 *	@brief A free standing class. Which is essentially a vector of std::functions
	 *
	 *	@note This class is threadsafe
	 */
	template<typename... TArgs>
	class EventSource
	{
	public:
		using Callable_t = std::function<void(TArgs...)>;
		using CallbackId = typename light_flat_value_map<Callable_t>::handle_t;

	private:
		light_flat_value_map<Callable_t> listeners;

	public:
		EventSource() = default;

		CallbackId add(const Callable_t& callback)
		{
			return listeners.push_back(callback);
		}

		CallbackId add(Callable_t&& callback)
		{
			return listeners.push_back(callback);
		}

		void remove(CallbackId id)
		{
			listeners.erase(id);
		}

		void call(TArgs... args)
		{
			for (auto& listener : listeners) {
				listener(args...);
			}
		}
	};
}
