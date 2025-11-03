#pragma once
#include"system.h"

namespace engine::system
{
	class SoundSystem final :public System
	{
	public:
		void init(Context& context)override;
	};
}