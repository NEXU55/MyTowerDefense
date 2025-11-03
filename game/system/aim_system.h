#pragma once
#include"../../engine/system/system.h"

namespace engine::component
{
	struct TransformComponent;
	struct PhysicsComponent;
	struct SpeedComponent;
	struct DamageComponent;
	struct RemoveComponent;
}
namespace game::component
{
	struct EnemyComponent;
	struct AimComponent;
}
namespace engine::event
{
	struct CollisionEvent;
}

namespace game::system
{
	class AimSystem final :public engine::system::System
	{
		using AimComponent = game::component::AimComponent;
		using TransformComponent = engine::component::TransformComponent;
		using PhysicsComponent = engine::component::PhysicsComponent;
		using SpeedComponent = engine::component::SpeedComponent;
		using DamageComponent = engine::component::DamageComponent;
		using RemoveComponent = engine::component::RemoveComponent; 

		using EnemyComponent = game::component::EnemyComponent; 
		using CollisionEvent = engine::event::CollisionEvent;
	public:
		void init(Context& context)override;
		void update(double, Context&)override;
	};
}