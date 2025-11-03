#pragma once
#include"system.h"
#include"../resource/animation_clip.h"
#include<unordered_map>
#include<string_view>

namespace engine::component
{
	struct SpriteComponent;
	struct AnimationComponent;
}

namespace engine::system
{
	//负责更新动画帧序列，确定精灵组件的源矩形
	class AnimationSystem final:public System
	{
		using SpriteComponent = engine::component::SpriteComponent;
		using AnimationClip = engine::resource::AnimationClip;
		using AnimationComponent = engine::component::AnimationComponent;
	public:
		void init(Context&)override;
		void update(double,Context&)override;
		void switch_to(Context& context,Entity entity,std::string_view name_animation,bool need_reset=true);
	};
}