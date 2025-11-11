#pragma once
#include <functional>
#include <vector>
#include <type_traits>
#include <memory>
#include <unordered_map>
#include<typeinfo>
#include<spdlog/spdlog.h>

namespace engine::core
{
	class EventBus
	{
	private:
		template<typename T>
		static std::vector<std::pair<std::weak_ptr<void>, std::function<void(const T&)>>>& _slots()
		{
			static std::vector<std::pair<std::weak_ptr<void>, std::function<void(const T&)>>> slots;
			return slots;
		}

		// 清理过期订阅的辅助函数
		template<typename T>
		static void _cleanupExpired()
		{
			auto& slots = _slots<T>();
			slots.erase(
				std::remove_if(slots.begin(), slots.end(),
					[](const auto& pair) { return pair.first.expired(); }),
				slots.end());
		}

	public:
		// 订阅事件类型 T，返回订阅令牌
		template <typename T>
		static std::shared_ptr<void> subscribe(std::function<void(const T&)> fn)
		{
			using Type = std::decay_t<T>;
			auto token = std::make_shared<int>(0); // 任意类型的共享指针作为令牌

			_slots<Type>().emplace_back(
				std::pair{ std::weak_ptr<void>(token), std::move(fn) });

			spdlog::trace("订阅事件类型{}", typeid(T).name());
			return token;
		}

		// 订阅事件类型 T，绑定到特定对象的生命周期
		template <typename T, typename Object>
		static std::shared_ptr<void> subscribe(Object* obj, void (Object::* method)(const T&))
		{
			using Type = std::decay_t<T>;

			// 创建与对象生命周期绑定的共享指针
			auto deleter = [](void*) { /* 空删除器，对象自己管理生命周期 */ };
			auto token = std::shared_ptr<void>(obj, deleter);

			auto fn = [obj, method](const T& event) { (obj->*method)(event); };

			_slots<Type>().emplace_back(
				std::pair{ std::weak_ptr<void>(token), std::move(fn) });

			return token;
		}

		// 发布事件
		template <typename T>
		static void publish(T&& e) {
			using Type = std::decay_t<T>;

			// 先清理过期订阅
			_cleanupExpired<Type>();

			auto& slots = _slots<Type>();
			// 调用有效的订阅函数
			for (const auto& [weakToken, fn] : slots) 
			{
				fn(e);
			}
		}

		// 清除所有该类型的事件监听
		template <typename T>
		static void clear() {
			using Type = std::decay_t<T>;
			_slots<Type>().clear();
		}

		// 手动取消订阅
		template <typename T>
		static void unsubscribe(const std::shared_ptr<void>& token) {
			using Type = std::decay_t<T>;
			auto& slots = _slots<Type>();

			slots.erase(
				std::remove_if(slots.begin(), slots.end(),
					[&token](const auto& pair) {
						return !pair.first.owner_before(token) && !token.owner_before(pair.first);
					}),
				slots.end());
		}
	};
}