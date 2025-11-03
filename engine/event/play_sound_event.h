#pragma once
#include<string>

namespace engine::event
{
	struct PlaySoundEventEvent
	{
		std::string type;
		std::string name;
	};
}