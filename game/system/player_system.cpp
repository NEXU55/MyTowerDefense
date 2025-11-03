#include "player_system.h"
#include"../../engine/core/context.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/event_bus.h"

#include"../../engine/component/component_manager.h"
#include"../../engine/component/transform_component.h"
#include"../../engine/component/speed_component.h"
#include"../../engine/component/physics_component.h"
#include"../../engine/component/animation_component.h"
#include"../../engine/component/effect_component.h"
#include"../../engine/component/sprite_component.h"

#include"../component/name_component.h"
#include"../component/player_component.h"

#include"../../engine/event/input_event.h"
#include"../../engine/event/animation_switch_event.h"
#include"../../engine/event/animation_finish_event.h"
#include"../event/create_player_effect_event.h"
#include"../event/player_mp_event.h"
#include <glm/geometric.hpp>

namespace game::system
{
	void PlayerSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto transform_type = coordinator.get_component_type<TransformComponent>();
		auto speed_type = coordinator.get_component_type<SpeedComponent>();
		auto physics_type = coordinator.get_component_type<PhysicsComponent>();
		auto animation_type = coordinator.get_component_type<AnimationComponent>();

		auto name_type = coordinator.get_component_type<NameComponent>();
		auto player_type = coordinator.get_component_type<PlayerComponent>();

		signature_.set(transform_type, true);
		signature_.set(speed_type, true);
		signature_.set(physics_type, true);
		signature_.set(animation_type, true);
		signature_.set(name_type, true);
		signature_.set(player_type, true);

		auto& event_bus = context.get_eventbus();
		auto p=event_bus.subscribe<engine::event::InputEvent>(
			[&context,&coordinator, this](const engine::event::InputEvent& event)
			{
				auto [entity, player] = coordinator.get_component_singleton<PlayerComponent>();

				if (player.state != PlayerComponent::State::Idle)
					return;
				if (player.can_attack && event.message == "player_attack")
					switch_to(PlayerComponent::State::Attack, entity, context);
				else if (player.can_release_skill && event.message == "player_release_skill")
					switch_to(PlayerComponent::State::ReleaseSkill, entity, context);
				else if (event.message == "player_move_left")
					player.is_move_left = true;
				else if (event.message == "player_move_right")
					player.is_move_right = true;
				else if (event.message == "player_move_up")
					player.is_move_up = true;
				else if (event.message == "player_move_down")
					player.is_move_down = true;
			});
		events_.emplace_back(p);

		p=event_bus.subscribe<engine::event::AnimationFinishEvent>(
			[&coordinator, &context, this](const engine::event::AnimationFinishEvent& event)
			{
				if (!coordinator.has_component<engine::component::EffectComponent>(event.entity))
					return;
				Entity entity = coordinator.get_component<engine::component::EffectComponent>(event.entity).owner;
				if (entity_list_.count(entity) == 0)
					return;
				switch_to(PlayerComponent::State::Idle, entity, context);
			});
		events_.emplace_back(p);
	}
	void PlayerSystem::update(double delta, Context& context)
	{
		auto& coordinator = context.get_coordinator();
		for (auto entity : entity_list_)
		{
			auto& player = coordinator.get_component<PlayerComponent>(entity);
			if (player.state != PlayerComponent::State::Idle)
				continue;
			auto& animation = coordinator.get_component<AnimationComponent>(entity);
			auto& physics = coordinator.get_component<PhysicsComponent>(entity);
			auto& speed = coordinator.get_component<SpeedComponent>(entity);
			
			double dir_x = player.is_move_right-player.is_move_left;
			double dir_y = player.is_move_down - player.is_move_up;
			player.is_move_left = 
			player.is_move_right = 
			player.is_move_up = 
			player.is_move_down = false;
			physics.velocity =dir_x==0&&dir_y==0?glm::dvec2{0,0}: glm::normalize(glm::dvec2{dir_x,dir_y})* speed.speed;
			if(dir_x != 0)
				player.facing = dir_x < 0 ? Direction::Left : Direction::Right;
			if (dir_y != 0)
				player.facing = dir_y < 0 ? Direction::Up : Direction::Down;
			animation.dir = player.facing;

			if (player.can_attack==false&&player.timer_attack.update(delta))
				player.can_attack = true;
			if(player.mp <= player.mp_max&&player.timer_increase_mp.update(delta))
			{
				player.mp += player.speed_recover_mp;
				if (player.mp >= player.mp_max)
				{
					player.mp = player.mp_max;
					player.can_release_skill = true;
				}
				game::event::PlayerMpEvent event;
				event.process = 1.0 * player.mp / player.mp_max;
				auto& event_bus = context.get_eventbus();
				event_bus.publish(event);
			}
		}
	}

	void PlayerSystem::switch_to(PlayerComponent::State state,Entity entity,Context& context)
	{
		auto& coordinator = context.get_coordinator();
		auto& event_bus = context.get_eventbus();

		auto& player = coordinator.get_component<PlayerComponent>(entity);
		auto& name = coordinator.get_component<game::component::NameComponent>(entity);

		player.state = state;

		engine::event::AnimationSwitchEvent animation_switch_event;
		animation_switch_event.entity = entity;
		if (state == PlayerComponent::State::Idle)
		{
			animation_switch_event.name_animation = name.name + "_idle";
		}
		else
		{
			auto& physics = coordinator.get_component<PhysicsComponent>(entity);
			physics.velocity = { 0,0 };

			auto& transform = coordinator.get_component<TransformComponent>(entity);
			auto& sprite = coordinator.get_component<engine::component::SpriteComponent>(entity);

			glm::dvec2 offset{0,0};
			switch (player.facing)
			{
			case Direction::Up:
				offset = glm::dvec2{ 0,-sprite.src_rect.h / 2 +30};
				break;
			case Direction::Down:
				offset = glm::dvec2{ 0,sprite.src_rect.h / 2 -30};
				break;
			case Direction::Left:
				offset = glm::dvec2{ -sprite.src_rect.w / 2,0 };
				break;
			case Direction::Right:
				offset = glm::dvec2{ sprite.src_rect.w / 2,0 };
				break;
			}

			animation_switch_event.name_animation = name.name + "_attack";
			game::event::CreatePlayerEffectEvent create_player_effect_event;
			create_player_effect_event.position = transform.position+offset;
			create_player_effect_event.dir = player.facing;
			create_player_effect_event.owner = entity;
			if (state == PlayerComponent::State::Attack)
			{
				player.can_attack = false;
				create_player_effect_event.type = game::event::CreatePlayerEffectEvent::EffectType::Impact;
			}
			else
			{
				player.can_release_skill = false;
				player.mp = 0;
				game::event::PlayerMpEvent event;
				event.process = 1.0 * player.mp / player.mp_max;
				auto& event_bus = context.get_eventbus();
				event_bus.publish(event);

				create_player_effect_event.type = game::event::CreatePlayerEffectEvent::EffectType::Flash;
			}
				
			event_bus.publish(create_player_effect_event);
		}
			
		event_bus.publish(animation_switch_event);
	}
}
