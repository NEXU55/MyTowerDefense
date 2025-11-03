#pragma once
#include<glm/vec2.hpp>

namespace engine::component
{
	struct PhysicsComponent
	{
		glm::dvec2 velocity{ 0,0 };
		bool can_out_of_bounds = false;
		bool is_out_of_bounds = false;
	};
}