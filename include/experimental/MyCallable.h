#pragma once
#include <cstdarg>
#include "TMPUtils.h"
#include <tuple>
#include <cassert>
#include <cstdarg>
#include <cstdarg>


namespace cof
{
	class InvokerBase
	{
	public:
		InvokerBase() = default;
		virtual ~InvokerBase() = default;

		virtual void Run(void* tupleArgs) = 0;


		//Disable all the copy/move constructors
		InvokerBase(const InvokerBase& other) = delete;
		InvokerBase(InvokerBase&& other) noexcept = delete;
		InvokerBase& operator=(const InvokerBase& other) = delete;
		InvokerBase& operator=(InvokerBase&& other) noexcept = delete;
	};

	template<typename TCall, typename... TArgs>
	class CallableInvoker : public InvokerBase {
		TCall call;

	public:
		CallableInvoker(TCall& callCopy) : call(callCopy)
		{
		}

		CallableInvoker(TCall&& call) : call(std::move(call))
		{
		}

		void Run(void* tupleArgs) override
		{
			/*va_list args;
			va_start(args, dummy);
			std::tuple<TArgs...> tuple;
			static_for<0, sizeof...(TArgs)> unrolled_loop{};
			unrolled_loop([&tuple, &args](auto i) {
				using T = std::tuple_element_t<i.value, std::tuple<TArgs...>>;
				std::get<i.value>(tuple) = va_arg(args, T);
			});*/

			//std::invoke(call, __crt_va_arg(args, TArgs)...);

			auto& tuple = *static_cast<std::tuple<TArgs...>*>(tupleArgs);
			cof::Apply(call, tuple);
			//va_end(args);
		}
	};

	template<typename TThis, typename TMemberFunc, typename... TArgs>
	class MemberFunctionInvoker : public InvokerBase {
		TThis* objectPtr;
		TMemberFunc functionPtr;

	public:
		MemberFunctionInvoker(TThis* thisPtr, TMemberFunc func) : objectPtr(thisPtr), functionPtr(func)
		{
			assert(thisPtr != nullptr);
		}

		void Run(void* tupleArgs) override
		{
			auto& tuple = *static_cast<std::tuple<TArgs...>*>(tupleArgs);

			auto& object_ref_local = *objectPtr;
			auto function_ptr_local = functionPtr;

			cof::Apply([&object_ref_local, function_ptr_local](TArgs&&... ts) {
				(object_ref_local.*function_ptr_local)(std::forward<TArgs>(ts)...);
			}, std::forward<std::tuple<TArgs...>>(tuple));

			//va_list args;
			//va_start(args, dummy);
			//RunImplementation(objectPtr, functionPtr, args);
			//va_end(args);
		}

	private:
		// static void RunImplementation(TThis* objectPtr, void (TThis::*functionPtr)(TArgs...), va_list args)
		// {
		// 	std::tuple<TArgs...> tuple;
		// 	static_for<0, sizeof...(TArgs)> unrolled_loop{};
		// 	unrolled_loop([&tuple, &args](auto i) {
		// 		using T = std::tuple_element_t<i.value, std::tuple<TArgs...>>;
		// 		T t = va_arg(args, T);
		// 		std::get<i.value>(tuple) = t;
		// 	});
		//
		// 	auto& object_ref_local = *objectPtr;
		// 	auto function_ptr_local = functionPtr;
		//
		// 	//(object_ref_local.*function_ptr_local)(va_arg(args, TArgs)...);
		//
		// 	std::apply([&object_ref_local, function_ptr_local](TArgs&&... ts) {
		// 		(object_ref_local.*function_ptr_local)(std::forward<TArgs>(ts)...);
		// 	}, std::forward<std::tuple<TArgs...>>(tuple));
		// }
	};
}
