#pragma once
#include<SDL_rect.h>

namespace game::event
{
	struct CreatePlayerEvent
	{
		SDL_Point index = { 0 };
	};
}