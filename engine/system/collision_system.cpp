#include "collision_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../component/component_manager.h"

#include"../component/transform_component.h"
#include"../component/collision_component.h"

#include <glm/geometric.hpp>

namespace engine::system
{
	void CollisionSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto collision_type = coordinator.get_component_type<CollisionComponent>();
		auto transform_type = coordinator.get_component_type<TransformComponent>();
		signature_.set(collision_type, true);
		signature_.set(transform_type, true);
	}

	void CollisionSystem::update(double delta, Context& context)
	{
		auto& coordinator = context.get_coordinator();
		auto& event_bus = context.get_eventbus();

		for (auto entity_src : entity_list_)
		{
			auto& collision_box_src = coordinator.get_component<CollisionComponent>(entity_src);
			auto& position_src = coordinator.get_component<TransformComponent>(entity_src).position;
			if (collision_box_src.enabled == false)
					continue;
			for (auto entity_dst : entity_list_)
			{
				auto& collision_box_dst = coordinator.get_component<CollisionComponent>(entity_dst);
				auto& position_dst = coordinator.get_component<TransformComponent>(entity_dst).position;
				if (collision_box_dst.enabled == false || entity_src==entity_dst || (collision_box_src.collision_mask & collision_box_dst.layer) == 0)
						continue;
				if (!is_collide(collision_box_src, collision_box_dst, position_src, position_dst))
					continue;
				CollisionEvent event;
				event.owner = collision_box_src.owner;
				event.target = collision_box_dst.owner;
				event.type = collision_box_src.event_type;
				event_bus.publish(event);
			}
		}
	}
	bool CollisionSystem::is_collide(const CollisionComponent& a, const CollisionComponent& b, const glm::dvec2& pos_a, const glm::dvec2& pos_b) const
	{
		if (a.type == CollisionBoxType::Rec)
		{
			if (b.type == CollisionBoxType::Rec)
				return check_rec_rec(a, b, pos_a, pos_b);
			else if (b.type == CollisionBoxType::Cir)
				return check_rec_cir(a, b, pos_a, pos_b);
		}
		else if (a.type == CollisionBoxType::Cir)
		{
			if (b.type == CollisionBoxType::Rec)
				return check_cir_rec(a, b, pos_a, pos_b);
			else if (b.type == CollisionBoxType::Cir)
				return check_cir_cir(a, b, pos_a, pos_b);
		}
		return false;
	}
	bool CollisionSystem::check_rec_rec(const CollisionComponent& a, const CollisionComponent& b, const glm::dvec2& pos_a, const glm::dvec2& pos_b) const
	{
		glm::dvec2 size_a{0,0};
		glm::dvec2 size_b{0,0};
		bool useable = true;

		std::visit([&size_a, &useable](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, glm::dvec2>)
					size_a = arg;
				else if constexpr (std::is_same_v<T, double>)
					useable = false;
			}, a.size);
		std::visit([&size_b, &useable](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, glm::dvec2>)
					size_b = arg;
				else if constexpr (std::is_same_v<T, double>)
					useable = false;
			}, b.size);
		if (!useable)
			return false;

		return std::max(pos_a.x + size_a.x / 2, pos_b.x + size_b.x / 2) - std::min(pos_a.x - size_a.x / 2, pos_b.x - size_b.x / 2) < size_a.x + size_b.x
			&& std::max(pos_a.y + size_a.y / 2, pos_b.y + size_b.y / 2) - std::min(pos_a.y - size_a.y / 2, pos_b.y - size_b.y / 2) < size_a.y + size_b.y;
	}
	bool CollisionSystem::check_cir_cir(const CollisionComponent& a, const CollisionComponent& b, const glm::dvec2& pos_a, const glm::dvec2& pos_b) const
	{
		double range_a = 0;
		double range_b = 0;
		bool useable = true;

		std::visit([&range_a, &useable](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, double>)
					range_a = arg;
				else if constexpr (std::is_same_v<T, glm::dvec2>)
					useable = false;
			}, a.size);
		std::visit([&range_b, &useable](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, double >)
					range_b = arg;
				else if constexpr (std::is_same_v<T, glm::dvec2>)
					useable = false;
			}, b.size);
		if (!useable)
			return false;

		return glm::length(pos_a - pos_b) < range_a + range_b;
	}
	bool CollisionSystem::check_cir_rec(const CollisionComponent& a, const CollisionComponent& b, const glm::dvec2& pos_a, const glm::dvec2& pos_b) const
	{
		double range_a = 0;
		glm::dvec2 size_b;
		bool useable = true;

		std::visit([&range_a, &useable](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, double>)
					range_a = arg;
				else if constexpr (std::is_same_v<T, glm::dvec2>)
					useable = false;
			}, a.size);
		std::visit([&size_b, &useable](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, glm::dvec2>)
					size_b = arg;
				else if constexpr (std::is_same_v<T, double>)
					useable = false;
			}, b.size);
		if (!useable)
			return false;

		return checkCircleRectangleCollision(pos_a.x, pos_a.y, range_a, pos_b.x-size_b.x/2, pos_b.y-size_b.y/2, size_b.x, size_b.y);
	}
}