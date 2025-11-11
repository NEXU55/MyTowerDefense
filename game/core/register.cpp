#include"register.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/context.h"

#include"../../engine/component/sprite_component.h"
#include"../../engine/component/transform_component.h"
#include"../../engine/component/animation_component.h"
#include"../../engine/component/physics_component.h"
#include"../../engine/component/speed_component.h"
#include"../../engine/component/health_component.h"
#include"../../engine/component/damage_component.h"
#include"../../engine/component/collision_component.h"
#include"../../engine/component/remove_component.h"
#include"../../engine/component/rotate_component.h"
#include"../../engine/component/effect_component.h"

#include"../component/map_component.h"
#include"../component/home_component.h"
#include"../component/enemy_component.h"
#include"../component/tower_component.h"
#include"../component/fire_component.h"
#include"../component/bullet_component.h"
#include"../component/aim_component.h"
#include"../component/name_component.h"
#include"../component/player_component.h"

#include"../../engine/system/sprite_system.h"
#include"../../engine/system/sound_system.h"
#include"../../engine/system/animation_system.h"
#include"../../engine/system/physics_system.h"
#include"../../engine/system/collision_system.h"
#include"../../engine/system/remove_system.h"
#include"../../engine/system/rotate_system.h"
#include"../../engine/system/effect_system.h"

#include"../system/enemy_system.h"
#include"../system/aim_system.h"
#include"../system/fire_system.h"
#include"../system/player_system.h"

namespace game::core
{
	void Register::register_all(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		coordinator.register_component<SpriteComponent>();
		coordinator.register_component<TransformComponent>();
		coordinator.register_component<AnimationComponent>();
		coordinator.register_component<PhysicsComponent>();
		coordinator.register_component<SpeedComponent>();
		coordinator.register_component<HealthComponent>();
		coordinator.register_component<DamageComponent>();
		coordinator.register_component<CollisionComponent>();
		coordinator.register_component<RemoveComponent>();
		coordinator.register_component<RotateComponent>();
		coordinator.register_component<EffectComponent>();

		coordinator.register_component_singleton<HomeComponent>();
		coordinator.register_component_singleton<MapComponent>();
		coordinator.register_component<EnemyComponent>();
		coordinator.register_component<TowerComponent>(256);
		coordinator.register_component<FireComponent>(256);
		coordinator.register_component<BulletComponent>(2048);
		coordinator.register_component<AimComponent>(2048);
		coordinator.register_component<PlayerComponent>(1);
		coordinator.register_component<game::component::NameComponent>();

		remove_system_ = coordinator.register_system<RemoveSystem>();
		sprite_system_=coordinator.register_system<SpriteSystem>();
		sound_system_ = coordinator.register_system<SoundSystem>();
		animation_system_=coordinator.register_system<AnimationSystem>();
		physics_system_ = coordinator.register_system<PhysicsSystem>();
		collision_system_ = coordinator.register_system<CollisionSystem>();
		
		rotate_system_ = coordinator.register_system<RotateSystem>();
		effect_system_ = coordinator.register_system<EffectSystem>();

		enemy_system_ = coordinator.register_system<EnemySystem>();
		aim_system_ = coordinator.register_system<AimSystem>(); 
		fire_system_= coordinator.register_system<FireSystem>();
		player_system_ = coordinator.register_system<PlayerSystem>();

		remove_system_->init(context);
		
		physics_system_->init(context);
		rotate_system_->init(context);
		collision_system_->init(context);

		sprite_system_->init(context);
		sound_system_->init(context);
		animation_system_->init(context);
		
		effect_system_->init(context);

		aim_system_->init(context);
		fire_system_->init(context);
		enemy_system_->init(context);
		player_system_->init(context);
	}

	void Register::update(double delta, Context& context)
	{
		
	}

	void Register::render(Context& context)
	{
		sprite_system_->render(context);
	}
}