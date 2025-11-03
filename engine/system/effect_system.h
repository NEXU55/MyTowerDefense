#pragma once
#include"system.h"

namespace engine::component
{
	struct EffectComponent;
	struct RemoveComponent;
}
namespace engine::system
{
	//负责游戏对象的运动
	class EffectSystem final :public System
	{
		using EffectComponent = engine::component::EffectComponent;
		using RemoveComponent = engine::component::RemoveComponent;
	public:
		void init(Context& context)override;
		void update(double, Context&)override;
	};
}