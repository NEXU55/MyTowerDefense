#pragma once
#include"../../engine/core/type.h"
#include"../../engine/core/event_bus.h"

#include"tower_type.h"
#include"enemy_type.h"
#include"bullet_type.h"

#include<glm/vec2.hpp>
#include<string_view>
#include<functional>

typedef struct SDL_Texture SDL_Texture;
namespace engine::core
{
	class Context;
}
namespace game::config
{
	class LevelConfig;
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
	struct NameComponent;
	struct PlayerComponent;
}

namespace game::system
{
	class FireSystem;
	class AimSystem;
}
namespace game::event
{
	struct PlaceTowerEvent;
	struct FireBulletEvent;
}

namespace game::object
{
	class ObjectManager
	{
		using Context=engine::core::Context;
		using LevelConfig = game::config::LevelConfig;
		
		using PlaceTowerEvent = game::event::PlaceTowerEvent;
		using FireBulletEvent = game::event::FireBulletEvent;

		using TransformComponent = engine::component::TransformComponent;
		using SpriteComponent = engine::component::SpriteComponent;
		using AnimationComponent = engine::component::AnimationComponent;
		using PhysicsComponent = engine::component::PhysicsComponent;
		using SpeedComponent = engine::component::SpeedComponent;
		using HealthComponent = engine::component::HealthComponent; 
		using DamageComponent = engine::component::DamageComponent;
		using CollisionComponent = engine::component::CollisionComponent; 
		using RemoveComponent = engine::component::RemoveComponent; 
		using RotateComponent = engine::component::RotateComponent;

		using EnemyComponent = game::component::EnemyComponent;
		using FireComponent = game::component::FireComponent;
		using TowerComponent = game::component::TowerComponent;
		using BulletComponent = game::component::BulletComponent;
		using AimComponent = game::component::AimComponent; 
		using NameComponent= game::component::NameComponent;
		using PlayerComponent = game::component::PlayerComponent;

		using FireSystem = game::system::FireSystem;
		using AimSystem = game::system::AimSystem;
	private:
		Context& context_;

		std::vector<std::shared_ptr<void>> events_;
		size_t count_enemy_ = 0;
		size_t count_bullet_ = 0;
		size_t count_tower_ = 0;

		std::vector<std::function<void()>> create_objects_;
	public:
		ObjectManager(Context& context);
		void update(double delta);

		size_t count_enemy()const { return count_enemy_; }
		size_t count_bullet()const { return count_bullet_; }
		size_t count_tower()const { return count_tower_; }
	public:
		void create_map(const LevelConfig&, SDL_Texture*);
		void create_home(const LevelConfig&);
		void create_enemy(EnemyType,int,glm::dvec2);
		void create_tower(TowerType, glm::dvec2);
		void create_bullet(BulletType type, double damage, Entity target_entity, glm::dvec2 position,glm::dvec2 direction);
		void create_explode(double damage,glm::dvec2 position);
		void create_player(glm::dvec2 position);
		void create_effect_flash(Entity owner, glm::dvec2 position, Direction dir);
		void create_effect_impact(Entity owner, glm::dvec2 position,Direction dir);
	};
}