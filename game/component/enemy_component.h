#pragma once
#include"../object/enemy_type.h"
#include"../../engine/component/timer.h"
#include<glm/vec2.hpp>

namespace game::component
{
	struct EnemyComponent
	{
		glm::dvec2 target{0,0};				//路线上下一个目标的坐标
		float process = 0;					//路线行进进度
		int index_target = 0;				//路线上下一个目标的索引
		int spawn_point = -1;				//路线
		int reward = 0;
		EnemyType type = EnemyType::None;	//敌人种类
		bool is_show_sketch = false;
		engine::component::Timer timer_sketch;
	};
}