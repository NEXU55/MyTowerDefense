#pragma once
#include"../../engine/scene/scene.h"
#include<memory>
#include <SDL.h>

namespace engine::ui
{
	class UIImage;
}

namespace game::scene
{
	class EndScene final :public engine::scene::Scene
	{
	private:
		bool is_win_ = true;

		const int speed_ = 200;
		engine::ui::UIImage* icon_ = nullptr;
	public:
		EndScene(Context&);
		~EndScene()override;
		void init()override;
		void update(double)override;
		void input(const InputEvent& event)override;
	private:
		void create_ui();
	};
}