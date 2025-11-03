#include "enemy_system.h"
#include"../../engine/core/context.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/event_bus.h"
#include"../core/wave_manager.h"

#include"../../engine/component/component_manager.h"
#include"../../engine/component/animation_component.h"
#include"../../engine/component/physics_component.h"
#include"../../engine/component/transform_component.h"
#include"../../engine/component/speed_component.h"
#include"../../engine/component/remove_component.h"
#include"../../engine/component/health_component.h"

#include"../component/name_component.h"
#include"../component/enemy_component.h"
#include"../../engine/event/animation_switch_event.h"
#include"../../engine/event/change_hp_event.h"
#include"../../engine/event/play_sound_event.h"
#include"../event/enemy_enter_home_event.h"
#include"../event/enemy_hp_event.h"
#include"../event/reward_event.h"
#include <glm/geometric.hpp>


namespace game::system
{
	void EnemySystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto animation_type = coordinator.get_component_type<AnimationComponent>();
		auto physics_type = coordinator.get_component_type<PhysicsComponent>();
		auto transform_type = coordinator.get_component_type<TransformComponent>();
		auto enemy_type = coordinator.get_component_type<EnemyComponent>();
		auto remove_type = coordinator.get_component_type<RemoveComponent>();

		signature_.set(animation_type, true);
		signature_.set(physics_type, true);
		signature_.set(transform_type, true);
		signature_.set(enemy_type, true);
		signature_.set(remove_type, true);

		auto& event_bus = context.get_eventbus();
		auto p=event_bus.subscribe<engine::event::ChangeHpEvent>(
			[this, &coordinator, &event_bus](const engine::event::ChangeHpEvent& event)
			{
				if (entity_list_.count(event.entity)==0||event.change >= 0)
					return;
				auto& name = coordinator.get_component<game::component::NameComponent>(event.entity);
				auto& enemy = coordinator.get_component<EnemyComponent>(event.entity);
				enemy.is_show_sketch = true;

				engine::event::AnimationSwitchEvent animation_switch_event;
				animation_switch_event.entity = event.entity;
				animation_switch_event.need_reset = false;
				animation_switch_event.name_animation = name.name + "_sketch";
				event_bus.publish(animation_switch_event);
			});
		events_.emplace_back(p);
	}
	void EnemySystem::update(double delta, Context& context, const WaveManager& wave_manager)
	{
		auto& coordinator = context.get_coordinator();
		auto& event_bus = context.get_eventbus();
		for (auto entity : entity_list_)
		{
			auto& animation = coordinator.get_component<AnimationComponent>(entity);
			auto& physics = coordinator.get_component<PhysicsComponent>(entity);
			auto& transform = coordinator.get_component<TransformComponent>(entity);
			auto& speed = coordinator.get_component<SpeedComponent>(entity);
			auto& health = coordinator.get_component<engine::component::HealthComponent>(entity);

			auto& enemy = coordinator.get_component<EnemyComponent>(entity);
			//向ui发送敌人血条信息
			game::event::EnemyHpEvent enemy_hp_event;
			enemy_hp_event.process = health.hp / health.hp_max;
			enemy_hp_event.position = transform.position;
			enemy_hp_event.entity = entity;
			event_bus.publish(enemy_hp_event);

			if (health.hp <= 0)
			{
				game::event::RewardEvent reward_event;
				reward_event.reward = enemy.reward;
				event_bus.publish(reward_event);
			}

			//当敌人越过目标地点时，纠正其位置
			//当到达目标地点时切换目标地点
			auto  target_distance = enemy.target - transform.position;
			if(target_distance.x > 0 && animation.dir == Direction::Left
			|| target_distance.x < 0 && animation.dir == Direction::Right
			|| target_distance.y > 0 && animation.dir == Direction::Up
			|| target_distance.y < 0 && animation.dir == Direction::Down
			|| glm::length(target_distance) <= 0.01)
			{	
				transform.position=enemy.target;
				if (!refresh_position_target(enemy, wave_manager))
				{
					auto& remove = coordinator.get_component<RemoveComponent>(entity);
					remove.can_remove = true;

					game::event::EnemyEnterHomeEvent event;
					event.entity = entity;
					event_bus.publish(event);

					engine::event::PlaySoundEventEvent play_sound_event;
					play_sound_event.type = "home";
					play_sound_event.name = "hurt";
					event_bus.publish(play_sound_event);
				}
				target_distance = enemy.target - transform.position;
				physics.velocity = glm::normalize(target_distance)*speed.speed;
				if (target_distance.x > 0)
					animation.dir = Direction::Right;
				else if (target_distance.x < 0)
					animation.dir = Direction::Left;
				else if (target_distance.y > 0)
					animation.dir = Direction::Down;
				else if (target_distance.y < 0)
					animation.dir = Direction::Up;
				else
					physics.velocity = { 0,0 };
			}

			if (!enemy.is_show_sketch)
				continue;
			if(!enemy.timer_sketch.update(delta))
				continue;
			auto& name = coordinator.get_component<game::component::NameComponent>(entity);

			engine::event::AnimationSwitchEvent animation_switch_event;
			animation_switch_event.entity = entity;
			animation_switch_event.name_animation = name.name + "_walk";
			event_bus.publish(animation_switch_event);
			enemy.timer_sketch.reset();
			enemy.is_show_sketch = false;
		}
	}

	bool EnemySystem::refresh_position_target(EnemyComponent& enemy,const WaveManager& wave_manager)
	{
		++enemy.index_target;
		enemy.process = (double)enemy.index_target / wave_manager.get_route_size(enemy.spawn_point);
		if (enemy.index_target >= wave_manager.get_route_size(enemy.spawn_point))
			return false;
		enemy.target=wave_manager.get_target_position(enemy.spawn_point, enemy.index_target);
		return true;
	}
}
