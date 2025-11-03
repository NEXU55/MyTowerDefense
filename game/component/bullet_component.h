#pragma once
#include"../object/bullet_type.h"

namespace game::component
{
	struct BulletComponent
	{
		BulletType type = BulletType::None;
	};
}