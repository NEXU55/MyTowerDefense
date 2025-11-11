#include "aim_system.h"
#include"../../engine/core/context.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/event_bus.h"

#include"../../engine/component/component_manager.h"
#include"../../engine/component/physics_component.h"
#include"../../engine/component/transform_component.h"
#include"../../engine/component/speed_component.h"
#include"../../engine/component/damage_component.h"
#include"../../engine//component/collision_component.h"
#include"../../engine/component/remove_component.h"
#include"../component/name_component.h"

#include"../component/aim_component.h"
#include"../component/bullet_component.h"

#include"../../engine/event/collision_event.h"
#include"../../engine/event/change_hp_event.h"
#include"../../engine/event/play_sound_event.h"
#include"../event/generate_explode_event.h"
#include <glm/geometric.hpp>


namespace game::system
{
	void AimSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto aim_type = coordinator.get_component_type<AimComponent>();
		auto physics_type = coordinator.get_component_type<PhysicsComponent>();
		auto transform_type = coordinator.get_component_type<TransformComponent>();
		auto speed_type = coordinator.get_component_type<SpeedComponent>();
		auto damage_type = coordinator.get_component_type<DamageComponent>();
		auto remove_type = coordinator.get_component_type<RemoveComponent>();

		signature_.set(aim_type, true);
		signature_.set(physics_type, true);
		signature_.set(transform_type, true);
		signature_.set(speed_type, true);
		signature_.set(damage_type, true);
		signature_.set(remove_type, true);

		auto& event_bus = context.get_eventbus();
		auto p=event_bus.subscribe<CollisionEvent>(
		[this, &coordinator,&event_bus](const CollisionEvent& event)
		{
			if (signature_ != event.type)
				return;
		
			auto& remove=coordinator.get_component<RemoveComponent>(event.owner);
			auto& damage = coordinator.get_component<DamageComponent>(event.owner);
			auto& collision = coordinator.get_component<engine::component::CollisionComponent>(event.owner);

			collision.enabled = false;
			remove.can_remove = true;

			engine::event::ChangeHpEvent decrease_hp_event;
			decrease_hp_event.change = -damage.damage;
			decrease_hp_event.entity = event.target;
			event_bus.publish(decrease_hp_event);

			auto& bullet = coordinator.get_component<game::component::BulletComponent>(event.owner);
			if (bullet.type == BulletType::Shell)
			{
				auto& transform = coordinator.get_component<TransformComponent>(event.owner);

				game::event::GenerateExplodeEvent explode_event;
				explode_event.position = transform.position;
				explode_event.damage = damage.damage;
				event_bus.publish(explode_event);
			}

			engine::event::PlaySoundEventEvent play_sound_event;
			play_sound_event.type = coordinator.get_component<game::component::NameComponent>(event.owner).name;
			play_sound_event.name = "hit";
			event_bus.publish(play_sound_event);
		});
		events_.emplace_back(p);
	}
	void AimSystem::update(double delta, Context& context)
	{
		auto& coordinator = context.get_coordinator();
		auto entity_list = coordinator.view(signature_);
		for (auto entity : entity_list)
		{
			auto& physics = coordinator.get_component<PhysicsComponent>(entity);
			if (physics.is_out_of_bounds)
			{
				auto& remove = coordinator.get_component<RemoveComponent>(entity);
				remove.can_remove = true;
				continue;
			}
			auto& aim = coordinator.get_component<AimComponent>(entity);
			auto target_entity = aim.targer_enemy_id;
			if (coordinator.is_entity_valid(target_entity) == false)
				continue;
			auto& transform = coordinator.get_component<TransformComponent>(entity);
			auto& speed = coordinator.get_component<SpeedComponent>(entity);

			auto& transform_enemy = coordinator.get_component<TransformComponent>(target_entity);
			if(transform_enemy.position != transform.position)
				physics.velocity = glm::normalize(transform_enemy.position - transform.position) * speed.speed;
		}
	}
}
