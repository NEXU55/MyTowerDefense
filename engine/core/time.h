#pragma once
#include<SDL.h>
#include<spdlog/spdlog.h>

namespace engine::core
{
	class Time final
	{
	private:
		double delta_ = 0.0;
		double time_scale_ = 1.0;

		int target_fps_ = 0;
		double target_frame_time_ = 0.0;
		Uint64 last_time_=0;
		Uint64 current_time_=0;
		Uint64 counter_freq_ = SDL_GetPerformanceFrequency();
	public:
		Time()
		{
			last_time_ = SDL_GetPerformanceCounter();
		}
		Time(const Time&) = delete;
		Time& operator=(const Time&) = delete;
		Time(Time&&) = delete;
		Time& operator=(Time&&) = delete;

		void on_update()
		{
			current_time_ = SDL_GetPerformanceCounter();
			double current_delta = (current_time_ - last_time_)*1e9/counter_freq_;
			if (current_delta< target_frame_time_ * 1e9)
			{
				Uint32 time_to_wait = static_cast<Uint32>((target_frame_time_ * 1e9 - current_delta)/1e6);
				SDL_Delay(time_to_wait);
			}
			delta_ = (SDL_GetPerformanceCounter()-last_time_) * 1e9 / counter_freq_/1e9;
			last_time_ = SDL_GetPerformanceCounter();
		}
		double get_delta()const { return delta_*time_scale_; }
		double get_unscale_delta()const { return delta_; }
		void set_time_scale(double scale) { time_scale_ = scale; }
		double get_time_scale()const { return time_scale_; }
		void set_target_fps(int fps) 
		{
			if (fps < 0)
				return;
			target_fps_ = fps; 
			target_frame_time_ = 1.0 / target_fps_; 
			spdlog::info("Target FPS 设置为: {} (Frame time: {:.6f}s)", target_fps_, target_frame_time_);
		}
		int get_target_fps()const { return target_fps_; }
	};
}