#include"remove_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../component/component_manager.h"
#include"../component/health_component.h"
#include"../component/remove_component.h"

#include"../core/event_bus.h"
#include"../event/change_hp_event.h"

namespace engine::system
{
	void RemoveSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto remove_type = coordinator.get_component_type<RemoveComponent>();
		signature_.set(remove_type, true);

		auto& event_bus = context.get_eventbus();
		auto p=event_bus.subscribe<ChangeHpEvent>(
		[&coordinator](const ChangeHpEvent& event)
		{
			Entity entity = event.entity;
			auto& health = coordinator.get_component<HealthComponent>(entity);
			health.hp += event.change;
			if (health.hp <= 0)
			{
				health.hp = 0;
				auto& remove = coordinator.get_component<RemoveComponent>(entity);
				remove.can_remove = true;
			}
			else if (health.hp > health.hp_max)
				health.hp = health.hp_max;

			spdlog::debug("实体{}当前生命值为{}", entity, health.hp);
		});
		events_.emplace_back(p);
	}

	void RemoveSystem::update(double delta, Context& context)
	{
		std::vector<Entity> remove_list;

		auto& coordinator = context.get_coordinator();
		for (auto entity : entity_list_)
		{
			auto& remove = coordinator.get_component<RemoveComponent>(entity);
			if (remove.can_remove)
				remove_list.emplace_back(entity);
		}

		for (auto entity : remove_list)
		{
			coordinator.destory_entity(entity);
		}
	}
}