#include "effect_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../core/event_bus.h"

#include"../component/component_manager.h"
#include"../component/remove_component.h"
#include"../component/effect_component.h"
#include"../component/damage_component.h"
#include"../component/collision_component.h"

#include"../event/animation_finish_event.h"
#include"../event/change_hp_event.h"
#include"../event/collision_event.h"
#include"../event/play_sound_event.h"

namespace engine::system
{
	void EffectSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();
		auto& event_bus = context.get_eventbus();

		auto effect_type = coordinator.get_component_type<EffectComponent>();
		auto remove_type = coordinator.get_component_type<RemoveComponent>();
		signature_.set(effect_type, true);
		signature_.set(remove_type, true);

		auto p=event_bus.subscribe<engine::event::CollisionEvent>(
			[this, &coordinator, &event_bus](const engine::event::CollisionEvent& event)
			{
				if (signature_ != event.type)
					return;
				auto& damage = coordinator.get_component<engine::component::DamageComponent>(event.owner);
				engine::event::ChangeHpEvent decrease_hp_event;
				decrease_hp_event.change = -damage.damage;
				decrease_hp_event.entity = event.target;
				event_bus.publish(decrease_hp_event);
			});
		events_.emplace_back(p);

		p=event_bus.subscribe<engine::event::AnimationFinishEvent>(
			[&coordinator, &event_bus, this](const engine::event::AnimationFinishEvent& event)
			{
				if (entity_list_.count(event.entity) == 0)
					return;
				auto& remove = coordinator.get_component<RemoveComponent>(event.entity);
				remove.can_remove = true;
			});
		events_.emplace_back(p);
	}
	void EffectSystem::update(double delta, Context& context)
	{
		auto& coordinator = context.get_coordinator();
		for (auto entity : entity_list_)
		{
			auto& effect = coordinator.get_component<EffectComponent>(entity);
			auto& damage = coordinator.get_component<engine::component::DamageComponent>(entity);
			if (!damage.one_shot)
				damage.damage = damage.damage_max * delta;
			if (effect.timer.update(delta))
			{
				auto& collision = coordinator.get_component<engine::component::CollisionComponent>(entity);
				collision.enabled = false;
			}
		}
	}
}
