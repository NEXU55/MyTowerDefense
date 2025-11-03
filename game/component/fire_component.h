#pragma once
#include"../../engine/core/type.h"
#include"../../engine/component/timer.h"

namespace game::component
{
	struct FireComponent
	{
		engine::component::Timer timer_fire;
		double target_enemy_process = 0;
		double range = 0;
		Entity target_enemy_id = -1;
		bool can_fire = true;
	};
}