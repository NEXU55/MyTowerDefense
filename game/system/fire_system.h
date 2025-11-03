#pragma once
#include"../../engine/system/system.h"

namespace engine::component
{
	struct TransformComponent;
	struct DamageComponent;
	struct AnimationComponent;
}
namespace game::component
{
	struct FireComponent;
	struct TowerComponent;
	struct EnemyComponent;
	struct NameComponent;
}
namespace engine::event
{
	struct CollisionEvent;
}
namespace game::event
{
	struct FireBulletEvent;
}

namespace game::system
{
	//实现防御塔索敌并产生发射子弹事件
	class FireSystem final :public engine::system::System
	{
		using TransformComponent = engine::component::TransformComponent;
		using DamageComponent = engine::component::DamageComponent;
		using AnimationComponent = engine::component::AnimationComponent;

		using NameComponent = game::component::NameComponent;
		using FireComponent = game::component::FireComponent;
		using TowerComponent = game::component::TowerComponent;
		using EnemyComponent = game::component::EnemyComponent; 

		using FireBulletEvent = game::event::FireBulletEvent;
		using CollisionEvent = engine::event::CollisionEvent;
	public:
		void init(Context& context)override;
		void update(double, Context&)override;

		void process_collide(const CollisionEvent& event,Context& context);
	};
}