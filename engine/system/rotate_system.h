#pragma once
#include"system.h"

namespace engine::component
{
	struct TransformComponent;
	struct PhysicsComponent;
	struct RotateComponent;
}

namespace engine::system
{
	class RotateSystem final :public System
	{
		using TransformComponent = engine::component::TransformComponent;
		using PhysicsComponent = engine::component::PhysicsComponent;
		using RotateComponent = engine::component::RotateComponent; 
	public:
		void init(Context& context)override;
		void update(double,Context&)override;
	};
}