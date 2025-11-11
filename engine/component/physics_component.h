#pragma once
#include<glm/vec2.hpp>

namespace engine::component
{
	struct PhysicsComponent
	{
		glm::dvec2 velocity{ 0,0 };			//速度
		bool can_out_of_bounds = false;		//可以出界
		bool is_out_of_bounds = false;		//是否出界
	};
}