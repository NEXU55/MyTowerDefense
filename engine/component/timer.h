#pragma once
#include<functional>

namespace engine::component
{
	struct Timer
	{
		bool one_shot_ = false;
		bool shotted_ = false;
		bool paused_ = false;
		double wait_time_ = 0;
		double pass_time_ = 0;

		void pause() { paused_ = true; }
		void resume() { paused_ = false; }

		void reset()
		{
			shotted_ = false;
			pass_time_ = 0;
		}

		bool update(double delta)
		{
			if (paused_)
				return false;
			pass_time_ += delta;
			if (pass_time_ < wait_time_)
				return false;
			bool can_shot = !(one_shot_ && shotted_);
			shotted_ = true;
			pass_time_ -= wait_time_;
			return can_shot;
		}
	};
}