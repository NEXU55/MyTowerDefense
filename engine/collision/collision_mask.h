#pragma once

namespace engine::collision
{
	constexpr auto NONE = 0;
	constexpr auto ENEMY = 1;
	constexpr auto BULLET = 1 << 1;
	constexpr auto TOWER = 1 << 2;
	constexpr auto PLAYER = 1 << 3;
	constexpr auto COIN = 1 << 4;
	constexpr auto HOME = 1 << 5;

	enum class CollisionBoxType
	{
		None = 0,
		Rec,
		Cir
	};
}