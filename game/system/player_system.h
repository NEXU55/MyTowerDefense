#pragma once
#include"../../engine/system/system.h"
#include"../component/player_component.h"

namespace engine::component
{
	struct TransformComponent;
	struct SpeedComponent;
	struct PhysicsComponent;
	struct AnimationComponent;
}
namespace game::component
{
	struct NameComponent;
}

namespace game::system
{
	//实现玩家移动、普攻、释放技能
	class PlayerSystem final :public engine::system::System
	{
		using TransformComponent = engine::component::TransformComponent;
		using SpeedComponent = engine::component::SpeedComponent;
		using PhysicsComponent = engine::component::PhysicsComponent;
		using AnimationComponent = engine::component::AnimationComponent;

		using NameComponent = game::component::NameComponent;
		using PlayerComponent = game::component::PlayerComponent;
	public:
		void init(Context& context)override;
		void update(double, Context&)override;
	private:
		void switch_to(PlayerComponent::State state, Entity entity, Context& context);
	};
}