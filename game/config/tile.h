#pragma once

namespace game::config
{
	constexpr auto SIZE_TILE = 48;		//瓦片大小
	struct Tile							//存储单个瓦片的信息，用于瓦片地图的渲染和预烘焙洋流图
	{
		enum class Direction			//敌人的移动方向
		{
			None = 0,
			Up,
			Down,
			Left,
			Right
		};

		int terrain = 0;				//索引
		int decoration = -1;			//装饰
		int special_flag = -1;			//特殊标记
		Direction dir = Direction::None;//敌人移动方向
	};
}