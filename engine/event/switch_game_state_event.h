#pragma once
#include"../core/game_state.h"
#include<string>

namespace engine::event
{
	struct SwitchGameStateEvent
	{
		engine::core::GameState state = engine::core::GameState::Playing;
		std::string action;
	};
}