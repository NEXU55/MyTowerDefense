#pragma once
#include"../../engine/component/timer.h"

namespace game::component
{
	struct PlayerComponent
	{
		enum class State
		{
			Idle=0,
			Attack,
			ReleaseSkill
		};

		engine::component::Timer timer_attack;
		engine::component::Timer timer_increase_mp;
		State state = State::Idle;
		Direction facing = Direction::Right;

		int mp = 0;
		int mp_max = 0;
		double speed_recover_mp = 0;

		bool is_move_left = false;
		bool is_move_right = false;
		bool is_move_up = false;
		bool is_move_down = false;
		bool can_attack = true;
		bool can_release_skill = false;
	};
}