#pragma once
#include<SDL.h>

namespace game::component
{
	struct HomeComponent
	{
		SDL_Point home_index = { 0 };	//家的索引
	};
}