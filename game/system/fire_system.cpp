#include "fire_system.h"
#include"../../engine/core/context.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/event_bus.h"

#include"../../engine/component/component_manager.h"
#include"../../engine/component/transform_component.h"
#include"../../engine/component/damage_component.h"
#include"../../engine/component/animation_component.h"

#include"../component/name_component.h"
#include"../component/fire_component.h"
#include"../component/tower_component.h"
#include"../component/enemy_component.h"

#include"../../engine/event/animation_switch_event.h"
#include"../../engine/event/animation_finish_event.h"
#include"../event/fire_bullet_event.h"
#include"../../engine/event/collision_event.h"
#include <glm/geometric.hpp>

namespace game::system
{
	void FireSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();
		
		auto transform_type = coordinator.get_component_type<TransformComponent>();
		auto damage_type = coordinator.get_component_type<DamageComponent>();
		auto animation_type = coordinator.get_component_type<AnimationComponent>();
		auto name_type = coordinator.get_component_type<NameComponent>();
		auto fire_type = coordinator.get_component_type<FireComponent>();
		auto tower_type = coordinator.get_component_type<TowerComponent>();
		signature_.set(transform_type, true);
		signature_.set(damage_type, true);
		signature_.set(animation_type, true);
		signature_.set(name_type, true);
		signature_.set(fire_type, true);
		signature_.set(tower_type, true);

		auto& event_bus = context.get_eventbus();
		auto p=event_bus.subscribe<CollisionEvent>(
			[&context, this](const CollisionEvent& event)
			{
				if (signature_ != event.type)
					return;
				process_collide(event, context);
			});
		events_.emplace_back(p);
		p=event_bus.subscribe<engine::event::AnimationFinishEvent>(
			[&coordinator , &event_bus, this](const engine::event::AnimationFinishEvent& event)
			{
				if (coordinator.has_component<game::component::TowerComponent>(event.entity) == 0)
					return;
				auto& name = coordinator.get_component<game::component::NameComponent>(event.entity);

				engine::event::AnimationSwitchEvent animation_switch_event;
				animation_switch_event.entity = event.entity;
				animation_switch_event.name_animation = name.name + "_idle";
				event_bus.publish(animation_switch_event);
			});
		events_.emplace_back(p);
	}
	void FireSystem::update(double delta, Context& context)
	{
		auto& coordinator = context.get_coordinator();
		auto& event_bus = context.get_eventbus();

		auto entity_list = coordinator.view(signature_);
		for (auto entity : entity_list)
		{
			auto& fire = coordinator.get_component<FireComponent>(entity);
			fire.can_fire = fire.can_fire||fire.timer_fire.update(delta);
			
			if (!coordinator.is_entity_valid(fire.target_enemy_id))
			{
				fire.target_enemy_id = -1;
				fire.target_enemy_process = 0;
				continue;
			}
			auto& tower = coordinator.get_component<TowerComponent>(entity);
			auto& damge = coordinator.get_component<DamageComponent>(entity);
			auto& transform = coordinator.get_component<TransformComponent>(entity);
			auto& transform_target = coordinator.get_component<TransformComponent>(fire.target_enemy_id);
			auto direction =  transform_target.position-transform.position;
			if (!fire.can_fire)
				continue;
			auto& name= coordinator.get_component<NameComponent>(entity);
			auto& animation= coordinator.get_component<AnimationComponent>(entity);
			if (abs(direction.x) > abs(direction.y))
				animation.dir = direction.x < 0 ? Direction::Left : Direction::Right;
			else
				animation.dir = direction.y < 0 ? Direction::Up : Direction::Down;
			
			engine::event::AnimationSwitchEvent animation_switch_event;
			animation_switch_event.entity = entity;
			animation_switch_event.name_animation = name.name+"_attack";
			event_bus.publish(animation_switch_event);

			FireBulletEvent fire_bullet_event;
			switch (tower.tower_type)
			{
			case TowerType::Archer:
				fire_bullet_event.type = BulletType::Arrow;
				break;
			case TowerType::Axeman:
				fire_bullet_event.type = BulletType::Axe;
				break;
			case TowerType::Gunner:
				fire_bullet_event.type = BulletType::Shell;
				break;
			}
			fire_bullet_event.position =transform.position ;
			fire_bullet_event.target_entity = fire.target_enemy_id;
			fire_bullet_event.damage = damge.damage;
			fire_bullet_event.direction = direction;

			event_bus.publish(fire_bullet_event);
			fire.can_fire = false;
			fire.timer_fire.reset();
			fire.target_enemy_id = -1;
			fire.target_enemy_process = 0;
		}
	}
	void FireSystem::process_collide(const CollisionEvent& event, Context& context)
	{
		Entity owner = event.owner;
		Entity target = event.target;
		auto& coordinator = context.get_coordinator();

		auto& fire = coordinator.get_component<FireComponent>(owner);
		auto& enemy = coordinator.get_component<EnemyComponent>(target);

		if (fire.target_enemy_process >= enemy.process || !fire.can_fire)
			return;
		fire.target_enemy_id = target;
		fire.target_enemy_process = enemy.process;
	}
}
