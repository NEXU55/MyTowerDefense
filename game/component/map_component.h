#pragma once
#include<SDL.h>

namespace game::component
{
	struct MapComponent
	{
		int width = 0;			//地图的横向的瓦片数量
		int height = 0;			//地图纵向的瓦片数量
		SDL_Point home_index = {0};	//家的索引
	};
}