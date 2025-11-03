#pragma once
#include"../../engine/system/system.h"

namespace game::core
{
	class WaveManager;
}
namespace engine::component
{
	struct AnimationComponent;
	struct TransformComponent;
	struct PhysicsComponent;
	struct SpeedComponent;
	struct RemoveComponent;
}
namespace game::component
{
	struct EnemyComponent;
}
namespace game::event
{
	struct SketchEvent;
}

namespace game::system
{
	//确保敌人按照路径行走
	class EnemySystem final:public engine::system::System
	{
		using WaveManager = game::core::WaveManager;

		using AnimationComponent = engine::component::AnimationComponent;
		using TransformComponent = engine::component::TransformComponent;
		using PhysicsComponent = engine::component::PhysicsComponent;
		using SpeedComponent = engine::component::SpeedComponent;
		using RemoveComponent = engine::component::RemoveComponent;

		using EnemyComponent = game::component::EnemyComponent;
		using SketchEvent=game::event::SketchEvent;
	public:
		void init(Context& context)override;
		void update(double, Context&,const WaveManager&);
	private:
		bool refresh_position_target(EnemyComponent& ,const WaveManager&);
	};
}