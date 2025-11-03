#pragma once
#include"../core/type.h"

namespace engine::event
{
	struct CollisionEvent
	{
		Entity owner = -1;
		Entity target = -1;
		Signature type;
	};
}