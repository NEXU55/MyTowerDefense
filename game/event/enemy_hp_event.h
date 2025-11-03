#pragma once
#include"../../engine/core/type.h"
#include<glm/vec2.hpp>
namespace game::event
{
	struct EnemyHpEvent
	{
		double process=0;
		glm::dvec2 position{ 0,0 };
		Entity entity = -1;
	};
}