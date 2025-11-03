#pragma once
#include"../core/context.h"
#include"../core/event_bus.h"
#include"../event/input_event.h"
#include<vector>
#include<string>
#include<string_view>
#include<functional>
#include<memory>

namespace engine::ui
{
	class UIManager;
}
namespace engine::system
{
	class PhysicsSystem;
	class CollisionSystem;
}
namespace engine::scene
{
	class Scene
	{
	protected:
		using InputEvent = engine::event::InputEvent;
		using Context = engine::core::Context;

		using PhysicsSystem = engine::system::PhysicsSystem;
		using CollisionSystem = engine::system::CollisionSystem;
	protected:
		//依赖项，隐式注入
		Context& context_;

		std::vector<std::shared_ptr<void>> events_;
		std::unique_ptr<engine::ui::UIManager> ui_manager_;
		std::string name_;							//场景名称
		bool is_initialized_ = false;               //场景是否已初始化(非当前场景很可能未被删除，因此需要初始化标志避免重复初始化)

		PhysicsSystem* physics_system_ = nullptr;
		CollisionSystem* collision_system_ = nullptr;
	public:
		Scene(Context& context, std::string_view name);
		virtual ~Scene();
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(Scene&&) = delete;

		// 核心循环方法
		virtual void init();                        //初始化场景。
		virtual void input(const InputEvent& event);//处理输入。
		virtual void update(double delta);			//更新场景
		virtual void render();                      //渲染场景。
		virtual void clear();						//清理场景。

		std::string get_name()const { return name_; }
		bool is_initialized()const { return is_initialized_; }
	protected:
		template<class T>
		void subscribe_event(std::function<void(const T&)> fn)
		{
			events_.emplace_back(context_.get_eventbus().subscribe(fn));
		}
	};
}