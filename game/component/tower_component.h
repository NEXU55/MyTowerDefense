#pragma once
#include"../object/tower_type.h"

namespace game::component
{
	struct TowerComponent
	{
		TowerType tower_type = TowerType::None;
		int level = -1;
		int upgrade_cost = 0;
	};
}