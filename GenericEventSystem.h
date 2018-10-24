#pragma once
#include <cstdarg>
#include <tuple>
#include "TMPUtils.h"

namespace cof
{
	struct EmptyStruct {};

	class InvokerBase
	{
	public:
		InvokerBase() = default;
		virtual ~InvokerBase() = default;

		virtual void Run(EmptyStruct dummy, ...) = 0;


		//Disable all the copy/move constructors
		InvokerBase(const InvokerBase& other) = delete;
		InvokerBase(InvokerBase&& other) noexcept = delete;
		InvokerBase& operator=(const InvokerBase& other) = delete;
		InvokerBase& operator=(InvokerBase&& other) noexcept = delete;
	};

	template<typename TCall, typename... Ts>
	class CallableInvoker : public InvokerBase {
		TCall call;

	public:
		CallableInvoker(TCall&& call) : call(call)
		{
		}
	
		void Run(EmptyStruct dummy, ...) override
		{
			va_list args;
			va_start(args, dummy);
			std::tuple<Ts...> tuple;
			static_for<0, sizeof...(Ts)> unrolled_loop{};
			unrolled_loop([&tuple, &args](auto i) {
				using T = std::tuple_element_t<i.value, std::tuple<Ts...>>;
				std::get<i.value>(tuple) = va_arg(args, T);
			});

			std::apply(call, tuple);
			va_end(args);

		}
	};



	template<typename TThis, typename TFunc>
	class MemberFunctionInvoker : public InvokerBase {
		TThis& object_ref;
		TFunc function_ptr;

	public:
		MemberFunctionInvoker(TThis& this_, TFunc func) : object_ref(this_), function_ptr(func)
		{
		}

		void Run(EmptyStruct dummy, ...) override
		{
			va_list args;
			va_start(args, dummy);
			RunImplementation(object_ref, function_ptr, args);
			va_end(args);
		}

	private:
		template<typename... TArgs>
		static void RunImplementation(TThis& object_ref, void (TThis::*function_ptr)(TArgs...), va_list args)
		{
			std::tuple<TArgs...> tuple;
			static_for<0, sizeof...(TArgs)> unrolled_loop{};
			unrolled_loop([&tuple, &args](auto i) {
				using T = std::tuple_element_t<i.value, std::tuple<TArgs...>>;
				std::get<i.value>(tuple) = va_arg(args, T);
			});

			auto& object_ref_local = object_ref;
			auto function_ptr_local = function_ptr;

			std::apply([&object_ref_local, function_ptr_local](TArgs&&... ts) {
			(object_ref_local.*function_ptr_local)(std::forward<TArgs>(ts)...);
			}, std::forward<std::tuple<TArgs...>>(tuple));
		}
	};
}