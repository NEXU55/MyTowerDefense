#pragma once
#include"../object/bullet_type.h"
#include<glm/vec2.hpp>

namespace game::event
{
	struct FireBulletEvent
	{
		glm::dvec2 position{ 0,0 };
		glm::dvec2 direction{ 0,0 };
		BulletType type=BulletType::None;
		Entity target_entity = -1;
		double damage = 0;
	};
}