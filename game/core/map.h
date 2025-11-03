#pragma once
#include"../config/tile.h"
#include<glm/vec2.hpp>
#include<vector>

namespace game::core
{
	class Map
	{
	private:
		struct Tile
		{
			bool can_place_tower = false;
			bool has_tower = false;
		};
	public:
		std::vector<std::vector<Tile>> map_;
	public:
		Map() = default;
		Map(std::vector<std::vector<Tile>>&& map)
			:map_(map)
		{}
		Map(size_t height, size_t width)
			:map_(height,std::vector<Tile>(width))
		{}

		bool can_place_tower(int y,int x)const
		{
			return map_[y][x].can_place_tower&&!map_[y][x].has_tower;
		}
		void place_tower(int y, int x)
		{
			map_[y][x].has_tower=true;
		}
	};
}