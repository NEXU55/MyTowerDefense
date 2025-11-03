#pragma once
#include"../../engine/core/type.h"
#include<glm/vec2.hpp>

namespace game::event
{
	struct CreatePlayerEffectEvent
	{
		enum class EffectType
		{
			Flash=0,
			Impact
		};

		Entity owner = -1;
		Direction dir = Direction::Right;
		EffectType type = EffectType::Flash;
		glm::dvec2 position{ 0,0 };
	};
}