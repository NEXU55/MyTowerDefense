#pragma once

namespace engine::core
{
	class Coordinator;
	class Context;
}

namespace engine::component
{
	struct SpriteComponent;
	struct TransformComponent;
	struct AnimationComponent;
	struct PhysicsComponent;
	struct SpeedComponent;
	struct HealthComponent;
	struct DamageComponent;
	struct CollisionComponent;
	struct RemoveComponent;
	struct RotateComponent;
	struct EffectComponent;
}

namespace engine::system
{
	class SpriteSystem;
	class SoundSystem;
	class AnimationSystem;
	class PhysicsSystem;
	class CollisionSystem;
	class RemoveSystem;
	class RotateSystem;
	class EffectSystem;
}

namespace game::component
{
	struct MapComponent;
	struct HomeComponent;
	struct EnemyComponent;
	struct TowerComponent;
	struct FireComponent;
	struct BulletComponent;
	struct AimComponent;
	struct PlayerComponent;
}

namespace game::system
{
	class EnemySystem;
	class AimSystem;
	class FireSystem;
	class PlayerSystem;
}

namespace game::core
{
	class Register
	{
		using SpriteComponent = engine::component::SpriteComponent;
		using TransformComponent = engine::component::TransformComponent;
		using AnimationComponent = engine::component::AnimationComponent;
		using PhysicsComponent = engine::component::PhysicsComponent;
		using SpeedComponent = engine::component::SpeedComponent;
		using HealthComponent = engine::component::HealthComponent;
		using DamageComponent = engine::component::DamageComponent;
		using CollisionComponent = engine::component::CollisionComponent; 
		using RemoveComponent = engine::component::RemoveComponent;
		using RotateComponent = engine::component::RotateComponent;
		using EffectComponent = engine::component::EffectComponent;

		using MapComponent = game::component::MapComponent;
		using HomeComponent = game::component::HomeComponent;
		using EnemyComponent = game::component::EnemyComponent;
		using TowerComponent = game::component::TowerComponent;
		using FireComponent = game::component::FireComponent;
		using BulletComponent = game::component::BulletComponent;
		using AimComponent = game::component::AimComponent; 
		using PlayerComponent = game::component::PlayerComponent;

		using SpriteSystem = engine::system::SpriteSystem;
		using SoundSystem = engine::system::SoundSystem;
		using AnimationSystem = engine::system::AnimationSystem;
		using PhysicsSystem = engine::system::PhysicsSystem;
		using CollisionSystem = engine::system::CollisionSystem;
		using RemoveSystem = engine::system::RemoveSystem;
		using RotateSystem = engine::system::RotateSystem;
		using EffectSystem = engine::system::EffectSystem;
		
		using EnemySystem = game::system::EnemySystem;
		using AimSystem = game::system::AimSystem;
		using FireSystem = game::system::FireSystem;
		using PlayerSystem = game::system::PlayerSystem;

		using Coordinator = engine::core::Coordinator;
		using Context = engine::core::Context;
	public:
		SpriteSystem* sprite_system_=nullptr;
		SoundSystem* sound_system_ = nullptr;
		AnimationSystem* animation_system_=nullptr;
		PhysicsSystem* physics_system_ = nullptr;
		CollisionSystem* collision_system_ = nullptr;
		RemoveSystem* remove_system_ = nullptr;
		RotateSystem* rotate_system_ = nullptr;
		EffectSystem* effect_system_ = nullptr;

		EnemySystem* enemy_system_ = nullptr;
		AimSystem* aim_system_ = nullptr;
		FireSystem* fire_system_ = nullptr; 
		PlayerSystem* player_system_ = nullptr;
	public:
		void register_all(Context& context);
		void update(double, Context&);
		void render(Context&);
	};
}