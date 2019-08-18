#pragma once
#include <cstdarg>
#include <tuple>
#include "TMPUtils.h"
#include "TemplateTypeId.h"
#include <unordered_map>
#include <memory>
#include <cassert>
#include <algorithm>
#include "CallbackHandle.h"
#include "EventDefinition.h"
#include "experimental/MyCallable.h"


namespace cof
{
	class GenericEventSystem
	{
	public:
		using CallbackHandleId = uint32_t;

	private:
		struct IdAndInvokerBase
		{
			CallbackHandleId id;
			std::unique_ptr<InvokerBase> invoker;

			IdAndInvokerBase(CallbackHandleId id, std::unique_ptr<InvokerBase> invoker) : id(id), invoker(std::move(invoker)) {}
		};

		using InvokerFlatMap = std::vector<IdAndInvokerBase>;

	public:

		GenericEventSystem() = default;

		template<typename TEvent, typename TCallable>
		CallbackHandle RegisterListener(TCallable&& callable)
		{
			return RegisterImpl<TEvent>(std::forward<TCallable>(callable), typename TEvent::TArgs{});
		}

		template<typename TEvent, typename TThis, typename TFunc>
		CallbackHandle RegisterListener(TThis* objectPtr, TFunc&& func)
		{
			return RegisterMemberFunctionImpl<TEvent>(objectPtr, func);
		}

		void Unregister(const CallbackHandle& handle)
		{
			Unregister(handle.EventTypeId(), handle.Id());
		}

		template<typename T>
		void Unregister(CallbackHandleId handle)
		{
			TemplateTypeId typeId = GetIdFromType<std::decay_t<T>>();
			auto it = callbackMap.find(typeId);
			//TODO: Should I still assert?
			//assert(it != callbackMap.end());
			if (it == callbackMap.end())
				return;

			InvokerFlatMap& invokerMap = it->second;
			InvokerFlatMap::iterator invokerIt = std::find_if(invokerMap.begin(), invokerMap.end(), [handle](IdAndInvokerBase& a) {
				return a.id == handle;
			});
			//TODO: Should I still assert?
			//assert(invokerIt != invokerMap.end());
			if(invokerIt != invokerMap.end()) {
				SwapToEndAndErase(invokerMap, invokerIt);
			}
		}

		void Unregister(TemplateTypeId eventTypeId, CallbackHandleId handle)
		{
			auto it = callbackMap.find(eventTypeId);
			//TODO: Should I still assert?
			//assert(it != callbackMap.end());
			if (it == callbackMap.end())
				return;

			InvokerFlatMap& invokerMap = it->second;
			InvokerFlatMap::iterator invokerIt = std::find_if(invokerMap.begin(), invokerMap.end(), [handle](IdAndInvokerBase& a) {
				return a.id == handle;
				});
			//TODO: Should I still assert?
			//assert(invokerIt != invokerMap.end());
			if (invokerIt != invokerMap.end()) {
				SwapToEndAndErase(invokerMap, invokerIt);
			}
		}

		void Unregister(CallbackHandleId handle)
		{
			for (auto it = callbackMap.begin(); it != callbackMap.end(); ++it) {
				InvokerFlatMap& flatMap = it->second;
				for (auto flatmapIt = flatMap.begin(); flatmapIt != flatMap.end(); ++flatmapIt) {
					if (flatmapIt->id == handle) {
						SwapToEndAndErase(flatMap, flatmapIt);
						return;
					}
				}
			}
		}

		// Template parameter @TEvent should be a definition from @EventDef found in "EventDefinition.h"
		template<typename TEvent, typename... TArgs>
		void FireEvent(TArgs&&... args)
		{
			static_assert(IsEventSignatureSame(typename TEvent::TArgs{}, TemplateParameterPack<TArgs...>{}), 
				"The arguments supplied are not the same as the arguments defined in TEvent::TArgs");

			TemplateTypeId eventId = GetIdFromType<TEvent>();
			auto it = callbackMap.find(eventId);
			assert(it != callbackMap.end());
			if(it != callbackMap.end()) {
				InvokerFlatMap& callableArray = it->second;

				std::tuple<TArgs...> tupleArgs{ std::forward<TArgs>(args)... };

				for (auto& idAndInvoker : callableArray) {
					idAndInvoker.invoker->Run(&tupleArgs);
				}
			}
		}

	private:
		template<typename... TEventDef, typename... TArgs>
		static constexpr bool IsEventSignatureSame(TemplateParameterPack<TEventDef...>, TemplateParameterPack<TArgs...>)
		{
			return sizeof...(TEventDef) == sizeof...(TArgs) && Conjunction_v<std::is_same<TEventDef, TArgs>...>;
		}

		static void SwapToEndAndErase(InvokerFlatMap& invokerMap, const InvokerFlatMap::iterator& iteratorToDelete)
		{
			auto& lastElement = invokerMap.back();
			std::swap(lastElement, *iteratorToDelete);
			invokerMap.erase(--invokerMap.end());
		}

		template<typename TEvent, typename TCallable, typename... TArgs>
		CallbackHandle RegisterImpl(TCallable callable, TemplateParameterPack<TArgs...>)
		{
			static_assert(IsInvocable_v<TCallable, TArgs...>(), 
				"TCallable cannot be called with the arguments that are specified in the TEvent it wants to register to!");

			InvokerBase* invoker = new CallableInvoker<TCallable, TArgs...>{ callable };

			TemplateTypeId typeId = GetIdFromType<std::decay_t<TEvent>>();
			CallbackHandleId callbackHandleId = ++handleIdCounter;
			// std::unordered_map::operator[] is noexcept and will create a default object if no entry existed yet.
			// Thus the following expression is safe.
			callbackMap[typeId].emplace_back(callbackHandleId, std::unique_ptr<InvokerBase>{invoker});

			return CallbackHandle{ callbackHandleId, typeId, this };
		}

		template<typename TEvent, typename TThis, typename... TArgs>
		CallbackHandle RegisterMemberFunctionImpl(TThis* thisPtr, void(TThis::*memberFunctionPtr)(TArgs...)){
			static_assert(IsEventSignatureSame(typename TEvent::TArgs{}, TemplateParameterPack<TArgs...>{}), 
				"Event signature needs to be the same as the event handlers function signature (That would make sense right? Because the handler will handle the event with those specific args!)");
			
			using MemberFunc = decltype(memberFunctionPtr);

			InvokerBase* invoker = new MemberFunctionInvoker<TThis, MemberFunc, TArgs...>(std::forward<TThis*>(thisPtr), std::forward<MemberFunc>(memberFunctionPtr));

			TemplateTypeId typeId = GetIdFromType<std::decay_t<TEvent>>();
			CallbackHandleId callbackHandleId = ++handleIdCounter;
			// std::unordered_map::operator[] is noexcept and will create a default object if no entry existed yet.
			// Thus the following expression is safe.
			callbackMap[typeId].emplace_back(callbackHandleId, std::unique_ptr<InvokerBase>{invoker});

			return CallbackHandle{ callbackHandleId, typeId, this };
		}

	private:
		std::unordered_map<TemplateTypeId, InvokerFlatMap> callbackMap;

		CallbackHandleId handleIdCounter = 0;
	};
}
