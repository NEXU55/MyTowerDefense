#pragma once
#include"../core/type.h"
#include<string>

namespace engine::event
{
	struct InputEvent
	{
		std::string message;						//消息
		ActionState state = ActionState::INACTIVE;	//状态
	};
}