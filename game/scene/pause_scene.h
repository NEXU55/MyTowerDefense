#pragma once
#include"../../engine/scene/scene.h"
#include<memory>
#include <SDL.h>

namespace engine::ui
{
	class UIButton;
}

namespace game::scene
{
	class PauseScene final :public engine::scene::Scene
	{
	private:
		engine::ui::UIButton* button_back_ = nullptr;
		engine::ui::UIButton* button_quit_ = nullptr;
	public:
		PauseScene(Context&);
		~PauseScene()override;
		void init()override;
	private:
		void create_ui();
	};
}