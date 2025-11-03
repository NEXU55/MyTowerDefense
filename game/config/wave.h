#pragma once
#include"../object/enemy_type.h"
#include<vector>

namespace game::config
{
	//表示一整个敌人波次
	struct Wave
	{
		//表示敌人波次中的一个生成事件
		struct SpawnEvent
		{
			double interval = 0;					//距离上一次事件间隔
			int spawn_point = 0;					//生成地点
			EnemyType enemy_type = EnemyType::None; //生成敌人种类
		};

		int rewards = 0;							//波次结束奖励
		double interval = 0;						//距离上一波时间间隔
		std::vector<SpawnEvent> spawn_event_list;	//波次的所有生成事件
	};
}