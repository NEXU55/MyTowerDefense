#pragma once
#include"../object/object_manager.h"
#include"../../engine/component/timer.h"
#include"../../engine/core/event_bus.h"
#include"../config/tile.h"
#include"../config/wave.h"

#include"../../engine/event/switch_game_state_event.h"
#include"../event/generate_enemy_event.h"
#include<unordered_map>
#include<SDL_rect.h>

namespace game::core
{
	class WaveManager final
	{
		using Timer = engine::component::Timer;
		using ObjectManager = game::object::ObjectManager;
		using EventBus = engine::core::EventBus;

		using Wave = game::config::Wave;
		using WaveList = std::vector<Wave>;
		using Route = std::vector<SDL_Point>;
		using RoutePool = std::unordered_map<int, Route>;
	private:
		ObjectManager& object_manager_;
		EventBus& event_bus_;

		WaveList wave_list_;
		RoutePool route_pool_;

		int index_wave_ = 0;
		int index_spawn_event_ = 0;
		Timer timer_wave_start_;
		Timer timer_spawn_event_;
		bool is_wave_started_ = false;
		bool is_spawned_last_enemy_ = false;
	public:
		WaveManager(ObjectManager& object_manager,EventBus& event_bus, 
			WaveList&& wave_list,RoutePool&& route_pool)
			:object_manager_(object_manager)
			,event_bus_(event_bus)
			,wave_list_(wave_list)
			,route_pool_(route_pool)
		{
			timer_wave_start_.one_shot_ = true;
			timer_wave_start_.wait_time_ = wave_list_[index_wave_].interval;
			timer_spawn_event_.one_shot_ = true;
		}

		size_t get_route_size(int spawn_point)const
		{
			const auto it = route_pool_.find(spawn_point);
			if (it == route_pool_.end())
				return 0;
			return it->second.size();
		}

		glm::dvec2 get_target_position(int spawn_point, int index)const
		{
			const auto it = route_pool_.find(spawn_point);
			if (it == route_pool_.end())
				return { 0,0 };
			const auto& point = it->second[index];
			return
			{
				point.x * config::SIZE_TILE + config::SIZE_TILE / 2,
				point.y * config::SIZE_TILE + config::SIZE_TILE / 2,
			};
		}

		void update(double delta)
		{
			//如果是最后一个生成事件且场上没有敌人
			if (is_spawned_last_enemy_&&object_manager_.count_enemy()==0)
			{
				//如果是最后一波
				if (++index_wave_ >= wave_list_.size())
				{
					engine::event::SwitchGameStateEvent event;
					event.state = engine::core::GameState::GameOver;
					event.action = "push";
					event_bus_.publish(event);
				}
				else
				{
					timer_wave_start_.wait_time_=wave_list_[index_wave_].interval;
					timer_wave_start_.reset();
					is_wave_started_ = is_spawned_last_enemy_ = false;
					index_spawn_event_ = 0;
				}
			}
			//如果波次未开始且波次计时器触发
			if (!is_wave_started_&&timer_wave_start_.update(delta))
			{
				is_wave_started_=true;
				timer_spawn_event_.wait_time_=wave_list_[index_wave_].spawn_event_list[index_spawn_event_].interval;
				timer_spawn_event_.reset();
			}
			//如果波次已开始且生成事件计时器触发
			if (is_wave_started_ && timer_spawn_event_.update(delta))
			{
				const auto& spawn_event_list = wave_list_[index_wave_].spawn_event_list;
				const auto& spawn_event = spawn_event_list[index_spawn_event_];

				//生成敌人
				game::event::GenerateEnemyEvent event;
				event.type = spawn_event.enemy_type;
				event.spawn_point = spawn_event.spawn_point;
				event.position = get_target_position(spawn_event.spawn_point,0);
				event_bus_.publish(event);

				//若为最后一次事件
				if (++index_spawn_event_ >= wave_list_[index_wave_].spawn_event_list.size())
					is_spawned_last_enemy_ = true;
				else
				{
					timer_spawn_event_.wait_time_=spawn_event_list[index_spawn_event_].interval;
					timer_spawn_event_.reset();
				}
			}
		}
	};
}


