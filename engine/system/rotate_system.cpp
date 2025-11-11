#include "rotate_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../component/component_manager.h"
#include"../component/rotate_component.h"
#include"../component/transform_component.h"
#include"../component/physics_component.h"

namespace engine::system
{
	void RotateSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto transform_type = coordinator.get_component_type<TransformComponent>();
		auto physics_type = coordinator.get_component_type<PhysicsComponent>();
		auto rotate_type = coordinator.get_component_type<RotateComponent>();

		signature_.set(transform_type, true);
		signature_.set(physics_type, true);
		signature_.set(rotate_type, true);
	}
	void RotateSystem::update(double delta, Context& context)
	{
		auto& coordinator = context.get_coordinator();
		auto entity_list = coordinator.view(signature_);
		for (auto entity : entity_list)
		{
			auto& rotate = coordinator.get_component<RotateComponent>(entity);
			if (!rotate.can_rotated)
				continue;
			auto& transform = coordinator.get_component<TransformComponent>(entity);
			auto& physics = coordinator.get_component<PhysicsComponent>(entity);
			double randian = std::atan2(physics.velocity.y, physics.velocity.x);
			transform.angle = randian * 180 / 3.1415926;
		}
	}
}

