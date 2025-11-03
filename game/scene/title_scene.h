#pragma once
#include"../../engine/scene/scene.h"
#include<memory>
#include <SDL.h>

namespace engine::ui
{
	class UIButton;
	class UIImage;
}

namespace game::scene
{
	class TitleScene final :public engine::scene::Scene
	{
	private:
		engine::ui::UIButton* button_start_ = nullptr;
		engine::ui::UIButton* button_quit_ = nullptr;
		engine::ui::UIImage* background_ = nullptr;
	public:
		TitleScene(Context&);
		~TitleScene()override;
		void init()override;
	private:
		void create_ui();
	};
}