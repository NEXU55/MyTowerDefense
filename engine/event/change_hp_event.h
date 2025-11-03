#pragma once
#include"../core/type.h"

namespace engine::event
{
	struct ChangeHpEvent
	{
		double change = 0;
		Entity entity = -1;
	};
}