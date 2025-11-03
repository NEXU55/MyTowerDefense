#include"object_manager.h"
#include"../../engine/core/context.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/config.h"
#include"../../engine/resource/resource_manager.h"
#include"../config/level_config.h"

#include"../../engine/collision/collision_mask.h"
#include"../../engine/event/play_sound_event.h"
#include"../event/place_tower_event.h"
#include"../event/create_player_event.h"
#include"../event/create_player_effect_event.h"
#include"../event/fire_bullet_event.h"
#include"../event/generate_explode_event.h"
#include"../event/generate_enemy_event.h"

#include"../../engine/resource/animation_clip.h"
#include"../../engine/component/animation_component.h"
#include"../../engine/component/sprite_component.h"
#include"../../engine/component/transform_component.h"
#include"../../engine/component/remove_component.h"
#include"../../engine/component/speed_component.h"
#include"../../engine/component/health_component.h"
#include"../../engine/component/damage_component.h"
#include"../../engine/component/physics_component.h"
#include"../../engine/component/collision_component.h"
#include"../../engine/component/rotate_component.h"
#include"../../engine/component/effect_component.h"

#include"../component/home_component.h"
#include"../component/map_component.h"
#include"../component/enemy_component.h"
#include"../component/fire_component.h"
#include"../component/tower_component.h"
#include"../component/aim_component.h"
#include"../component/bullet_component.h"
#include"../component/name_component.h"
#include"../component/player_component.h"

#include"../system/fire_system.h"
#include"../system/aim_system.h"
#include"../../engine/system/effect_system.h"
#include<string_view>

namespace game::object
{
	ObjectManager::ObjectManager(Context& context)
		:context_(context)
	{
		
		auto p=context_.get_eventbus().subscribe<PlaceTowerEvent>(
			[this](const PlaceTowerEvent& event)
			{
				auto type = event.type;
				auto tile_size = game::config::SIZE_TILE;
				glm::dvec2 position =
				{
					event.index.x * tile_size + tile_size / 2,
					event.index.y * tile_size + tile_size / 2
				};
				create_objects_.emplace_back([this,type,position]()
					{
						create_tower(type, position);
					});
			});
		events_.emplace_back(p);

		p=context_.get_eventbus().subscribe<game::event::CreatePlayerEvent>(
			[this](const game::event::CreatePlayerEvent& event)
			{
				auto tile_size = game::config::SIZE_TILE;
				glm::dvec2 position =
				{
					event.index.x * tile_size + tile_size / 2,
					event.index.y * tile_size + tile_size / 2
				};
				create_objects_.emplace_back([this,position]()
					{
						create_player(position); 
					});
			});
		events_.emplace_back(p);

		p=context_.get_eventbus().subscribe<FireBulletEvent>(
			[this](const FireBulletEvent& event)
			{
				create_objects_.emplace_back([this,event]()
					{
						create_bullet(event.type,event.damage,event.target_entity,event.position,event.direction);
					});
			});
		events_.emplace_back(p);

		p=context_.get_eventbus().subscribe<game::event::GenerateExplodeEvent>(
			[this](const game::event::GenerateExplodeEvent& event)
			{
				create_objects_.emplace_back([this,event]()
					{
						create_explode(event.damage,event.position);
					});
			});
		events_.emplace_back(p);

		p=context_.get_eventbus().subscribe<game::event::CreatePlayerEffectEvent>(
			[this](const game::event::CreatePlayerEffectEvent& event)
			{
				if (event.type == game::event::CreatePlayerEffectEvent::EffectType::Flash)
					create_objects_.emplace_back([this,event]()
					{
						create_effect_flash(event.owner,event.position, event.dir);
					});
					
				else
					create_objects_.emplace_back([this, event]()
					{
						create_effect_impact(event.owner, event.position, event.dir);
					});
			});
		events_.emplace_back(p);

		p=context_.get_eventbus().subscribe<game::event::GenerateEnemyEvent>(
			[this](const game::event::GenerateEnemyEvent& event)
			{
				create_objects_.emplace_back([this, event]()
					{
						create_enemy(event.type, event.spawn_point, event.position);
					});
			});
		events_.emplace_back(p);
	}

	void ObjectManager::update(double delta)
	{
		auto& coordinator = context_.get_coordinator();

		for (auto& create_object : create_objects_)
		{
			create_object();
		}
		create_objects_.clear();

		count_bullet_=coordinator.get_component_size<BulletComponent>();
		count_enemy_ = coordinator.get_component_size<EnemyComponent>();
		count_tower_ = coordinator.get_component_size<TowerComponent>();
	}

	void ObjectManager::create_map(const game::config::LevelConfig& level_config, SDL_Texture* texture)
	{
		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		//添加地图实体
		Entity map_entity = coordinator.create_entity();

		game::component::MapComponent map;
		map.width = level_config.get_tilemap_width();
		map.height = level_config.get_tilemap_height();
		map.home_index = level_config.get_index_home();
		coordinator.add_component(map_entity, map);

		engine::component::SpriteComponent map_sprite;
		map_sprite.layer = engine::component::SpriteComponent::SpriteLayer::Tile;
		map_sprite.texture = texture;
		map_sprite.size = resource_manager.get_texture_size(map_sprite.texture);
		map_sprite.src_rect = { 0,0,map_sprite.size.x,map_sprite.size.y };
		coordinator.add_component(map_entity, map_sprite);

		engine::component::TransformComponent map_transform;
		coordinator.add_component(map_entity, map_transform);
	}
	void ObjectManager::create_home(const game::config::LevelConfig& level_config)
	{
		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		auto& config = context_.get_config();
		//添加家实体
		Entity home_entity = coordinator.create_entity();

		game::component::HomeComponent home;
		home.home_index = level_config.get_index_home();
		coordinator.add_component(home_entity, home);

		engine::component::SpriteComponent home_sprite;
		home_sprite.layer = engine::component::SpriteComponent::SpriteLayer::Tile;
		home_sprite.texture = resource_manager.get_texture("home");
		home_sprite.size = resource_manager.get_texture_size(home_sprite.texture);
		home_sprite.src_rect = { 0,0,home_sprite.size.x,home_sprite.size.y };
		coordinator.add_component(home_entity, home_sprite);

		engine::component::TransformComponent home_transform;
		home_transform.position.x = home.home_index.x * game::config::SIZE_TILE;
		home_transform.position.y = home.home_index.y * game::config::SIZE_TILE;
		coordinator.add_component(home_entity, home_transform);
	}
	void ObjectManager::create_enemy(EnemyType type, int spawn_point, glm::dvec2 position)
	{
		std::string name;
		switch (type)
		{
		case EnemyType::Slime:
			name = "enemy_slime";
			break;
		case EnemyType::RoyalSlime:
			name = "enemy_royal_slime";
			break;
		case EnemyType::Skeleton:
			name = "enemy_skeleton";
			break;
		case EnemyType::Goblin:
			name = "enemy_goblin";
			break;
		case EnemyType::GoblinPriest:
			name = "enemy_goblin_priest";
			break;
		}

		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		auto& config = context_.get_config().config_.enemies;

		Entity entity = coordinator.create_entity();

		TransformComponent transform;
		transform.position = position;
		coordinator.add_component(entity, transform);

		AnimationComponent animation;
		animation.current_clip = resource_manager.get_animation(name+"_walk");
		coordinator.add_component(entity, animation);

		SpriteComponent sprite;
		sprite.texture = resource_manager.get_texture(name);
		sprite.layer = SpriteComponent::SpriteLayer::Object;
		sprite.mode = SpriteComponent::AnchorMode::CENTER;
		size_t index = static_cast<size_t>(animation.dir);
		sprite.src_rect = animation.current_clip->frame[index][animation.idx_frame];
		sprite.size = { sprite.src_rect.w,sprite.src_rect.h };
		coordinator.add_component(entity, sprite);

		PhysicsComponent physics;
		coordinator.add_component(entity,physics);

		SpeedComponent speed;
		HealthComponent health;
		EnemyComponent enemy;
		auto it=config.find(name);
		if (it == config.end())
			spdlog::error("未找到该敌人的配置信息'{}'", name);
		else
		{
			speed.speed_max = speed.speed = it->second.speed;
			health.hp_max = health.hp = it->second.hp;
			enemy.reward = it->second.reward;
		}
		coordinator.add_component(entity, speed);
		coordinator.add_component(entity, health);

		enemy.spawn_point = spawn_point;
		enemy.type = type;
		enemy.target = position;
		enemy.timer_sketch.one_shot_ = true;
		enemy.timer_sketch.wait_time_ = 0.2;
		coordinator.add_component(entity, enemy);

		CollisionComponent collision;
		collision.layer = engine::collision::ENEMY;
		collision.collision_mask = engine::collision::BULLET;
		collision.type = engine::collision::CollisionBoxType::Rec;
		collision.owner = entity;
		collision.size = glm::dvec2{16,24};
		coordinator.add_component(entity, collision);

		NameComponent name_component;
		name_component.name = name;
		coordinator.add_component(entity, name_component);
		
		RemoveComponent remove;
		coordinator.add_component(entity, remove);
	}
	void ObjectManager::create_tower(TowerType type, glm::dvec2 position)
	{
		std::string name;
		switch (type)
		{
		case TowerType::Archer:
			name = "tower_archer";
			break;
		case TowerType::Axeman:
			name = "tower_axeman";
			break;
		case TowerType::Gunner:
			name = "tower_gunner";
			break;
		}

		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		auto& config = context_.get_config().config_.towers;

		Entity entity = coordinator.create_entity();

		TransformComponent transform;
		transform.position = position;
		coordinator.add_component(entity, transform);

		AnimationComponent animation;
		animation.current_clip = resource_manager.get_animation(name + "_idle");
		coordinator.add_component(entity, animation);

		SpriteComponent sprite;
		sprite.texture = resource_manager.get_texture(name);
		sprite.layer = SpriteComponent::SpriteLayer::Object;
		sprite.mode = SpriteComponent::AnchorMode::CENTER;
		size_t index = static_cast<size_t>(animation.dir);
		sprite.src_rect = animation.current_clip->frame[index][animation.idx_frame];
		sprite.size = { sprite.src_rect.w,sprite.src_rect.h };
		coordinator.add_component(entity, sprite);

		RemoveComponent remove;
		coordinator.add_component(entity, remove);

		FireComponent fire;
		TowerComponent tower;
		DamageComponent damage;

		auto it = config.find(name);
		if (it == config.end())
			spdlog::error("未找到该防御塔的配置信息'{}'", name);
		else
		{
			++tower.level;
			tower.upgrade_cost = it->second.upgrade_cost[tower.level];
			tower.tower_type = type;
			
			fire.range = it->second.view_range[tower.level];
			fire.timer_fire.wait_time_ = it->second.interval[tower.level];
			fire.timer_fire.one_shot_ = false;

			damage.damage_max = damage.damage = it->second.damage[tower.level];
		}
		coordinator.add_component(entity, fire);
		coordinator.add_component(entity, tower);
		coordinator.add_component(entity, damage);

		CollisionComponent collision;
		collision.layer = engine::collision::TOWER;
		collision.collision_mask = engine::collision::ENEMY;
		collision.type = engine::collision::CollisionBoxType::Cir;
		collision.owner = entity;
		collision.size = fire.range;
		collision.event_type = coordinator.get_signature<FireSystem>();
		coordinator.add_component(entity, collision);

		NameComponent name_component;
		name_component.name = name;
		coordinator.add_component(entity, name_component);

		engine::event::PlaySoundEventEvent play_sound_event;
		play_sound_event.type = "tower";
		play_sound_event.name = "place";
		context_.get_eventbus().publish(play_sound_event);
	}
	void ObjectManager::create_bullet(BulletType type,double damage,Entity target_entity, glm::dvec2 position,glm::dvec2 direction)
	{
		std::string name;
		switch (type)
		{
		case BulletType::Arrow:
			name = "bullet_arrow";
			break;
		case BulletType::Axe:
			name = "bullet_axe";
			break;
		case BulletType::Shell:
			name = "bullet_shell";
			break;
		}

		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		auto& config = context_.get_config().config_.bullets;

		Entity entity = coordinator.create_entity();

		TransformComponent transform;
		transform.position = position;
		coordinator.add_component(entity, transform);

		AnimationComponent animation;
		animation.current_clip = resource_manager.get_animation(name);
		coordinator.add_component(entity, animation);

		SpriteComponent sprite;
		sprite.texture = resource_manager.get_texture(name);
		sprite.layer = SpriteComponent::SpriteLayer::Object;
		sprite.mode = SpriteComponent::AnchorMode::CENTER;
		size_t index = static_cast<size_t>(animation.dir);
		sprite.src_rect = animation.current_clip->frame[index][animation.idx_frame];
		sprite.size = { sprite.src_rect.w,sprite.src_rect.h };
		coordinator.add_component(entity, sprite);

		SpeedComponent speed;
		RotateComponent rotate;
		auto it = config.find(name);
		if (it == config.end())
			spdlog::error("未找到该子弹的配置信息'{}'", name);
		else
		{
			speed.speed_max = speed.speed = it->second.speed;
			rotate.can_rotated = it->second.can_rotated;
		}
		coordinator.add_component(entity, speed);
		coordinator.add_component(entity, rotate);

		DamageComponent damage_component;
		damage_component.damage_max = damage_component.damage = damage;
		coordinator.add_component(entity, damage_component);

		PhysicsComponent physics;
		physics.can_out_of_bounds = true;
		physics.velocity = direction * speed.speed;
		coordinator.add_component(entity, physics);

		RemoveComponent remove;
		coordinator.add_component(entity, remove);

		BulletComponent bullet;
		bullet.type = type;
		coordinator.add_component(entity, bullet);

		AimComponent aim;
		aim.targer_enemy_id = target_entity;
		coordinator.add_component(entity, aim);

		CollisionComponent collision;
		collision.layer = engine::collision::BULLET;
		collision.collision_mask = engine::collision::ENEMY;
		collision.type = engine::collision::CollisionBoxType::Cir;
		collision.owner = entity;
		collision.size = 1.0;
		collision.event_type = coordinator.get_signature<AimSystem>();
		coordinator.add_component(entity, collision);

		NameComponent name_component;
		name_component.name = name;
		coordinator.add_component(entity, name_component);

		engine::event::PlaySoundEventEvent play_sound_event;
		play_sound_event.type = name;
		play_sound_event.name = "fire";
		context_.get_eventbus().publish(play_sound_event);
	}
	void ObjectManager::create_explode(double damage, glm::dvec2 position)
	{
		std::string name="effect_explode";

		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();

		Entity entity = coordinator.create_entity();

		TransformComponent transform;
		transform.position = position;
		coordinator.add_component(entity, transform);

		AnimationComponent animation;
		animation.current_clip = resource_manager.get_animation(name);
		coordinator.add_component(entity, animation);

		SpriteComponent sprite;
		sprite.texture = resource_manager.get_texture(name);
		sprite.layer = SpriteComponent::SpriteLayer::Effect;
		sprite.mode = SpriteComponent::AnchorMode::CENTER;
		size_t index = static_cast<size_t>(animation.dir);
		sprite.src_rect = animation.current_clip->frame[index][animation.idx_frame];
		sprite.size = { sprite.src_rect.w,sprite.src_rect.h };
		coordinator.add_component(entity, sprite);

		DamageComponent damage_component;
		damage_component.damage_max = damage_component.damage = damage;
		coordinator.add_component(entity, damage_component);

		RemoveComponent remove;
		coordinator.add_component(entity, remove);

		CollisionComponent collision;
		collision.layer = engine::collision::BULLET;
		collision.collision_mask = engine::collision::ENEMY;
		collision.type = engine::collision::CollisionBoxType::Cir;
		collision.owner = entity;
		collision.size = 48.0;
		collision.event_type = coordinator.get_signature<engine::system::EffectSystem>();
		coordinator.add_component(entity, collision);

		engine::component::EffectComponent effect;
		effect.timer.one_shot_ = true;
		effect.timer.wait_time_ = 0.025;
		coordinator.add_component(entity, effect);
	}
	void ObjectManager::create_player(glm::dvec2 position)
	{
		std::string name="player";

		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		auto& config = context_.get_config().config_.player;

		Entity entity = coordinator.create_entity();

		TransformComponent transform;
		transform.position = position;
		coordinator.add_component(entity, transform);

		AnimationComponent animation;
		animation.current_clip = resource_manager.get_animation(name + "_idle");
		coordinator.add_component(entity, animation);

		SpriteComponent sprite;
		sprite.texture = resource_manager.get_texture(name);
		sprite.layer = SpriteComponent::SpriteLayer::Player;
		sprite.mode = SpriteComponent::AnchorMode::CENTER;
		size_t index = static_cast<size_t>(animation.dir);
		sprite.src_rect = animation.current_clip->frame[index][animation.idx_frame];
		sprite.size = { sprite.src_rect.w,sprite.src_rect.h };
		coordinator.add_component(entity, sprite);

		SpeedComponent speed;
		speed.speed_max = speed.speed = config.speed;
		coordinator.add_component(entity, speed);

		PhysicsComponent physics;
		coordinator.add_component(entity, physics);

		NameComponent name_component;
		name_component.name = name;
		coordinator.add_component(entity, name_component);

		PlayerComponent player;
		player.mp_max = player.mp = 100;
		player.timer_attack.wait_time_ = config.normal_attack_interval;
		player.timer_increase_mp.wait_time_ = config.skill_interval;
		player.speed_recover_mp = config.skill_recover_speed;
		coordinator.add_component(entity, player);
	}
	void ObjectManager::create_effect_flash(Entity owner, glm::dvec2 position,Direction dir)
	{
		std::string name = "effect_flash";

		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		auto& config = context_.get_config().config_.player;

		Entity entity = coordinator.create_entity();

		AnimationComponent animation;
		animation.current_clip = resource_manager.get_animation(name);
		
		SpriteComponent sprite;
		sprite.texture = resource_manager.get_texture(name);
		sprite.layer = SpriteComponent::SpriteLayer::Effect;
		sprite.mode = SpriteComponent::AnchorMode::CENTER;
		size_t index = static_cast<size_t>(animation.dir);
		sprite.src_rect = animation.current_clip->frame[index][animation.idx_frame];
		sprite.size = { sprite.src_rect.w,sprite.src_rect.h };

		TransformComponent transform;
		transform.position = position;

		CollisionComponent collision;
		collision.layer = engine::collision::BULLET;
		collision.collision_mask = engine::collision::ENEMY;
		collision.type = engine::collision::CollisionBoxType::Rec;
		collision.owner = entity;
		collision.event_type = coordinator.get_signature<engine::system::EffectSystem>();
		
		switch (dir)
		{
		case Direction::Up:
			position;
			transform.angle = 270;
			transform.position += glm::dvec2{ 0,-sprite.src_rect.w/2  };
			collision.size = glm::dvec2{ sprite.src_rect.h,sprite.src_rect.w };
			break;
		case Direction::Down:
			transform.angle = 90;
			transform.position += glm::dvec2{ 0,sprite.src_rect.w/2  };
			collision.size = glm::dvec2{ sprite.src_rect.h,sprite.src_rect.w };
			break;
		case Direction::Left:
			transform.angle = 180;
			transform.position += glm::dvec2{ -sprite.src_rect.w / 2,0 };
			collision.size = glm::dvec2{ sprite.src_rect.h,sprite.src_rect.h };
			break;
		case Direction::Right:
			transform.angle = 0;
			transform.position += glm::dvec2{ sprite.src_rect.w / 2,0 };
			collision.size = glm::dvec2{ sprite.src_rect.w,sprite.src_rect.h };
			break;	
		}
		coordinator.add_component(entity, animation);
		coordinator.add_component(entity, sprite);
		coordinator.add_component(entity, transform);
		coordinator.add_component(entity, collision);

		DamageComponent damage_component;
		damage_component.damage_max = damage_component.damage = config.skill_damage;
		damage_component.one_shot = false;
		coordinator.add_component(entity, damage_component);

		RemoveComponent remove;
		coordinator.add_component(entity, remove);

		engine::component::EffectComponent effect;
		effect.timer.paused_ = true;
		effect.owner = owner;
		coordinator.add_component(entity, effect);

		engine::event::PlaySoundEventEvent play_sound_event;
		play_sound_event.type = name;
		play_sound_event.name = "effect";
		context_.get_eventbus().publish(play_sound_event);
	}
	void ObjectManager::create_effect_impact(Entity owner,glm::dvec2 position,Direction dir)
	{
		std::string name = "effect_impact";

		auto& coordinator = context_.get_coordinator();
		auto& resource_manager = context_.get_resource_manager();
		auto& config = context_.get_config().config_.player;

		Entity entity = coordinator.create_entity();

		AnimationComponent animation;
		animation.current_clip = resource_manager.get_animation(name);

		SpriteComponent sprite;
		sprite.texture = resource_manager.get_texture(name);
		sprite.layer = SpriteComponent::SpriteLayer::Effect;
		sprite.mode = SpriteComponent::AnchorMode::CENTER;
		size_t index = static_cast<size_t>(animation.dir);
		sprite.src_rect = animation.current_clip->frame[index][animation.idx_frame];
		sprite.size = { sprite.src_rect.w,sprite.src_rect.h };

		TransformComponent transform;
		transform.position = position;

		CollisionComponent collision;
		collision.layer = engine::collision::BULLET;
		collision.collision_mask = engine::collision::ENEMY;
		collision.type = engine::collision::CollisionBoxType::Rec;
		collision.owner = entity;
		collision.event_type = coordinator.get_signature<engine::system::EffectSystem>();

		switch (dir)
		{
		case Direction::Up:
			transform.angle = 270;
			transform.position += glm::dvec2{ 0,-sprite.src_rect.w / 2 };
			collision.size = glm::dvec2{ sprite.src_rect.h,sprite.src_rect.w };
			break;
		case Direction::Down:
			transform.angle = 90;
			transform.position += glm::dvec2{ 0,sprite.src_rect.w / 2 };
			collision.size = glm::dvec2{ sprite.src_rect.h,sprite.src_rect.w };
			break;
		case Direction::Left:
			transform.angle = 180;
			transform.position += glm::dvec2{ -sprite.src_rect.w / 2,0 };
			collision.size = glm::dvec2{ sprite.src_rect.w,sprite.src_rect.h };
			break;
		case Direction::Right:
			transform.angle = 0;
			transform.position += glm::dvec2{ sprite.src_rect.w / 2,0 };
			collision.size = glm::dvec2{ sprite.src_rect.w,sprite.src_rect.h };
			break;
		}
		coordinator.add_component(entity, animation);
		coordinator.add_component(entity, sprite);
		coordinator.add_component(entity, transform);
		coordinator.add_component(entity, collision);

		DamageComponent damage_component;
		damage_component.damage_max = damage_component.damage = config.normal_attack_damage;
		coordinator.add_component(entity, damage_component);

		RemoveComponent remove;
		coordinator.add_component(entity, remove);

		engine::component::EffectComponent effect;
		effect.timer.wait_time_=0.025;
		effect.owner = owner;
		coordinator.add_component(entity, effect);

		engine::event::PlaySoundEventEvent play_sound_event;
		play_sound_event.type = name;
		play_sound_event.name = "effect";
		context_.get_eventbus().publish(play_sound_event);
	}
}                                 