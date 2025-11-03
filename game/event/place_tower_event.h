#pragma once
#include"../object/tower_type.h"
#include<SDL_rect.h>

namespace game::event
{
	struct PlaceTowerEvent
	{
		TowerType type = TowerType::None;
		SDL_Point index{ 0,0 };
	};
}