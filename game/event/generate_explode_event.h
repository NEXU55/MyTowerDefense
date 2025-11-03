#pragma once
#include<glm/vec2.hpp>

namespace game::event
{
	struct GenerateExplodeEvent
	{
		glm::dvec2 position{ 0,0 };
		double damage = 0;
	};
}

