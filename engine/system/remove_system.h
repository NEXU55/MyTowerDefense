#pragma once
#include"system.h"

namespace engine::component
{
	struct HealthComponent;
	struct RemoveComponent;
}

namespace engine::event
{
	struct ChangeHpEvent;
}

namespace engine::system
{
	//负责更新动画帧序列，确定精灵组件的源矩形
	class RemoveSystem final :public System
	{
		using HealthComponent = engine::component::HealthComponent;
		using RemoveComponent = engine::component::RemoveComponent;
		using ChangeHpEvent = engine::event::ChangeHpEvent;
	public:
		void init(Context& context)override;
		void update(double, Context&)override;
	};
}