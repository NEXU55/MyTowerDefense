#pragma once
#include"system.h"

namespace engine::component
{
	struct SpriteComponent;
	struct TransformComponent;
}

namespace engine::system 
{
	class SpriteSystem final:public System
	{
		using SpriteComponent = engine::component::SpriteComponent;
		using TransformComponent = engine::component::TransformComponent;
	public:
		void init(Context& context)override;
		void render(Context&);
	};
}