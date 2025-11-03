#pragma once
#include"../core/type.h"

namespace engine::event
{
	struct AnimationFinishEvent
	{
		Entity entity = -1;
	};
}