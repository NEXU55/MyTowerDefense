#pragma once
#include<functional>

namespace engine::component
{
	struct Timer
	{
		bool one_shot_ = false;					//是否为单次触发
		bool shotted_ = false;					//是否触发
		bool paused_ = false;					//暂停
		double wait_time_ = 0;					//等待时间
		double pass_time_ = 0;					//已过时间

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