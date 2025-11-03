#pragma once
#include"../object/enemy_type.h"
#include<glm/vec2.hpp>

namespace game::event
{
	struct GenerateEnemyEvent
	{
		EnemyType type=EnemyType::None;
		int spawn_point=-1;
		glm::dvec2 position{ 0,0 };
	};
}