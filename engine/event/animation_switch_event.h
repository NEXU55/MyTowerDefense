#pragma once
#include"../core/type.h"
#include<string>

namespace engine::event
{
	struct AnimationSwitchEvent
	{
		Entity entity = -1;
		bool need_reset = true;
		std::string name_animation;
	};
}