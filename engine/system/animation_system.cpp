#include"animation_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../../engine/core/event_bus.h"
#include"../../engine/resource/resource_manager.h"

#include"../component/component_manager.h"
#include"../component/sprite_component.h"
#include"../component/animation_component.h"

#include"../event/animation_finish_event.h"
#include"../event/animation_switch_event.h"

namespace engine::system
{
	void AnimationSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto sprite_type = coordinator.get_component_type<SpriteComponent>();
		auto animation_type = coordinator.get_component_type<AnimationComponent>();
		signature_.set(sprite_type, true);
		signature_.set(animation_type, true);

		auto& event_bus = context.get_eventbus();
		auto p=event_bus.subscribe<engine::event::AnimationSwitchEvent>(
		[this,&context](const engine::event::AnimationSwitchEvent& event) 
		{
			switch_to(context,event.entity, event.name_animation,event.need_reset);
		});
		events_.emplace_back(p);
	}

	void AnimationSystem::update(double delta,Context& context)
	{
		auto& coordinator = context.get_coordinator();
		auto entity_list = coordinator.view(signature_);
		for (auto entity : entity_list)
		{
			auto& animation = coordinator.get_component<AnimationComponent>(entity);
			animation.pass_time += delta;
			AnimationClip* clip = animation.current_clip;
			if (clip == nullptr)
			{
				spdlog::error("动画片段指针为空");
				continue;
			}

			size_t index = static_cast<size_t>(animation.dir);
			const auto& frame = clip->frame[index];
			auto& sprite = coordinator.get_component<SpriteComponent>(entity);
			sprite.src_rect = frame[animation.idx_frame];

			if (animation.pass_time < clip->duration)
				continue;
			animation.pass_time -= clip->duration;

			if (++animation.idx_frame < frame.size())
				continue;
			animation.idx_frame =clip->is_loop?0:frame.size()-1;
			if(!clip->is_loop)
			{
				auto& event_bus = context.get_eventbus();
				engine::event::AnimationFinishEvent event;
				event.entity = entity;
				event_bus.publish(event);
			}
		}
	}

	void AnimationSystem::switch_to(Context& context,Entity entity, std::string_view name_animation, bool need_reset)
	{
		auto& coordinator = context.get_coordinator();
		auto& resource_manager = context.get_resource_manager();

		auto& animation = coordinator.get_component<AnimationComponent>(entity);
		if (need_reset)
		{
			animation.idx_frame = 0;
			animation.pass_time = 0;
		}
		animation.current_clip = resource_manager.get_animation(name_animation);

		auto& sprite = coordinator.get_component<SpriteComponent>(entity);
		sprite.texture = animation.current_clip->texture;

		spdlog::debug("切换至动画{}", name_animation);
	}
}