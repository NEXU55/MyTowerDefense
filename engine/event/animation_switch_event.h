#pragma once
#include"../core/type.h"
#include<string>

namespace engine::event
{
	struct AnimationSwitchEvent
	{
		Entity entity = -1;
		bool need_reset = true;			//动画切换后帧索引是否重置
		std::string name_animation;		//切换到哪个动画
	};
}