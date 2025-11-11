#pragma once

//碰撞掩码
namespace engine::collision
{
	constexpr auto NONE = 0;			//无
	constexpr auto ENEMY = 1;			//敌人
	constexpr auto BULLET = 1 << 1;		//子弹
	constexpr auto TOWER = 1 << 2;		//塔
	constexpr auto PLAYER = 1 << 3;		//玩家
	constexpr auto COIN = 1 << 4;		//金币
	constexpr auto HOME = 1 << 5;		//家
//碰撞盒类型
	enum class CollisionBoxType
	{
		None = 0,
		Rec,		//矩形
		Cir			//圆形
	};
}