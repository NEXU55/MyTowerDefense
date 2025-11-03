#pragma once
#include"../core/type.h"
#include<string>

namespace engine::event
{
	struct InputEvent
	{
		std::string message;
		ActionState state = ActionState::INACTIVE;
	};
}