#include "collision_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../component/component_manager.h"

#include"../component/transform_component.h"
#include"../component/collision_component.h"

#include <glm/geometric.hpp>
#include<deque>

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
		int n = coordinator.get_component_size<engine::component::CollisionComponent>();
		std::vector<int> v;
		std::deque<int> q;
		std::vector<std::pair<int,int>> result;
		v.reserve(n);

		auto entity_list = coordinator.view(signature_);
		auto collision_list_ptr = coordinator.get_component_array<engine::component::CollisionComponent>();
		if (!collision_list_ptr)
		{
			spdlog::error("碰撞系统无法使用");
			return;
		}
		auto& collision_list = collision_list_ptr->get_component_array();
		auto size = collision_list_ptr->get_size();

		/*for (int i = 0; i < size; ++i)
		{
			if (collision_list[i].enabled == false)
				continue;
			const auto& position = coordinator.get_component<TransformComponent>(collision_list[i].owner).position;
			double width = 0.0, height = 0.0;
			std::visit([&width, &height](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, double>)
						width = height = arg;
					else if constexpr (std::is_same_v<T, glm::dvec2>)
					{
						width = arg.x / 2;
						height = arg.y / 2;
					}
				}, collision_list[i].size);
			collision_list[i].rect = { position.x - width,position.x + width,position.y - height,position.y + height };
		}
		for (int i = 0; i < size; ++i)
		{
			if (collision_list[i].enabled == false)
				continue;
			for (int j = 0; j < size; ++j)
			{
				if (collision_list[j].enabled == false)
					continue;
				const auto& collision_box1 = collision_list[i];
				const auto& collision_box2 = collision_list[j];

				bool need_to_publish_event1 = (collision_box1.collision_mask & collision_box2.layer) != 0;
				bool need_to_publish_event2 = (collision_box2.collision_mask & collision_box1.layer) != 0;

				if ((!need_to_publish_event1 && !need_to_publish_event2) || !is_collide(collision_box1, collision_box2))
					continue;
				if (need_to_publish_event1)
					result.emplace_back(i, j);
				if (need_to_publish_event2)
					result.emplace_back(j, i);
			}
		}*/

		for (int i=0;i<size;++i)
		{
			if (collision_list[i].enabled == false)
				continue;
			const auto& position = coordinator.get_component<TransformComponent>(collision_list[i].owner).position;
			double width = 0.0,height=0.0;
			std::visit([&width,&height](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, double>)
						width=height = arg;
					else if constexpr (std::is_same_v<T, glm::dvec2>)
					{
						width = arg.x / 2;
						height = arg.y/2;
					}
				}, collision_list[i].size);
			collision_list[i].rect = { position.x - width,position.x + width,position.y - height,position.y + height };
			v.emplace_back(i);
		}

		std::sort(v.begin(), v.end(),
			[&collision_list,this](int a, const int b)
			{return model?collision_list[a].rect.left < collision_list[b].rect.left: collision_list[a].rect.up < collision_list[b].rect.up; });

		for (auto e1 : v)
		{
			while (true)
			{
				if (!q.empty() && (model?collision_list[q.front()].rect.right < collision_list[e1].rect.left: 
					collision_list[q.front()].rect.down < collision_list[e1].rect.up))
					q.pop_front();
				else
					break;
			};
			for (auto e2 : q)
			{
				const auto& collision_box1 = collision_list[e1];
				const auto& collision_box2 = collision_list[e2];

				bool need_to_publish_event1 = (collision_box1.collision_mask & collision_box2.layer) != 0;
				bool need_to_publish_event2 = (collision_box2.collision_mask & collision_box1.layer) != 0;

				if ((!need_to_publish_event1 && !need_to_publish_event2)||!is_collide(collision_box1, collision_box2))
					continue;
				if (need_to_publish_event1)
					result.emplace_back(e1, e2);
				if (need_to_publish_event2)
					result.emplace_back(e2, e1);
			}
			q.emplace_back(e1);
		}

		for (const auto& [e1, e2] : result)
		{
			const auto& collision_box1 = collision_list[e1];
			const auto& collision_box2 = collision_list[e2];
			if (!collision_box1.enabled || !collision_box2.enabled)
				continue;
			engine::event::CollisionEvent event;
			event.owner = collision_box1.owner;
			event.target = collision_box2.owner;
			event.type = collision_box1.event_type;
			event_bus.publish(event);
		}
	}

	bool CollisionSystem::is_collide(const CollisionComponent& a, const CollisionComponent& b) const
	{
		if (a.type == CollisionBoxType::Rec)
		{
			if (b.type == CollisionBoxType::Rec)
				return check_rec_rec(a, b);
			else if (b.type == CollisionBoxType::Cir)
				return check_rec_cir(a, b);
		}
		else if (a.type == CollisionBoxType::Cir)
		{
			if (b.type == CollisionBoxType::Rec)
				return check_cir_rec(a, b);
			else if (b.type == CollisionBoxType::Cir)
				return check_cir_cir(a, b);
		}
		return false;
	}
	bool CollisionSystem::check_rec_rec(const CollisionComponent& a, const CollisionComponent& b) const
	{
		return model ? 
			std::max(a.rect.down,b.rect.down) - std::min(a.rect.up, b.rect.up) < a.rect.down+b.rect.down-b.rect.up-a.rect.up:
			std::max(a.rect.right, b.rect.right) - std::min(a.rect.left, b.rect.left) < a.rect.right + b.rect.right - b.rect.left - a.rect.left;
	}
	bool CollisionSystem::check_cir_cir(const CollisionComponent& a, const CollisionComponent& b) const
	{
		return glm::length(glm::dvec2{a.rect.right-a.rect.left+b.rect.left-b.rect.right,a.rect.down - a.rect.up + b.rect.up - b.rect.down }) <
			2*(a.rect.right - a.rect.left + b.rect.right - b.rect.left);
	}
	bool CollisionSystem::check_cir_rec(const CollisionComponent& a, const CollisionComponent& b) const
	{
		double range_a = (a.rect.right-a.rect.left)/2;
		return checkCircleRectangleCollision((a.rect.right + a.rect.left) / 2, (a.rect.down + a.rect.up) / 2, range_a,b.rect.left,b.rect.right,b.rect.up,b.rect.down);
	}
}