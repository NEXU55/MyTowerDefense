#include "physics_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../core/config.h"

#include"../component/component_manager.h"
#include"../component/physics_component.h"
#include"../component/transform_component.h"

namespace engine::system
{
	void PhysicsSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto physics_type = coordinator.get_component_type<PhysicsComponent>();
		auto transform_type = coordinator.get_component_type<TransformComponent>();
		signature_.set(physics_type, true);
		signature_.set(transform_type, true);
	}

	void PhysicsSystem::update(double delta, Context& context)
	{
		auto& coordinator = context.get_coordinator();
		const auto& window = context.get_config().config_.basic.window;
		for (auto entity : entity_list_)
		{
			auto& physics = coordinator.get_component<PhysicsComponent>(entity);
			auto& transform = coordinator.get_component<TransformComponent>(entity);

			transform.position += physics.velocity * delta;
			if(transform.position.x<0
			|| transform.position.x>window.window_width
			|| transform.position.y<0
			|| transform.position.y>window.window_height)
				physics.is_out_of_bounds = true;

			if (physics.can_out_of_bounds||!physics.is_out_of_bounds)
				continue;

			if (transform.position.x < 0)
				transform.position.x = 0;
			else if(transform.position.x > window.window_width)
				transform.position.x = window.window_width;
			if (transform.position.y < 0)
				transform.position.y = 0;
			else if (transform.position.y > window.window_height)
				transform.position.y = window.window_height;

			physics.is_out_of_bounds = false;
		}
	}
}
