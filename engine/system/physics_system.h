#pragma once
#include"system.h"

namespace engine::component
{
	struct TransformComponent;
	struct PhysicsComponent;
}
namespace engine::system
{
	//负责游戏对象的运动
	class PhysicsSystem final :public System 
	{
		using TransformComponent = engine::component::TransformComponent;
		using PhysicsComponent = engine::component::PhysicsComponent;
	public:
		void init(Context& context)override;
		void update(double, Context&)override;
	};
}