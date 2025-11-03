#include"pause_scene.h"

#include"../../engine/core/context.h"
#include"../../engine/core/event_bus.h"
#include"../../engine/render/renderer.h"
#include"../../engine/resource/resource_manager.h"

#include"../../engine/ui/ui_manager.h"
#include"../../engine/ui/ui_button.h"
#include"../../engine/ui/ui_panel.h"

#include"../../engine/event/switch_game_state_event.h"

#include<spdlog/spdlog.h>

namespace game::scene
{
	PauseScene::PauseScene(Context& context)
		: Scene(context, "pause scene")
	{
	}
	PauseScene::~PauseScene() = default;

	void PauseScene::init()
	{
		Scene::init();
		create_ui();
	}

	void PauseScene::create_ui()
	{
		int width = 0, height = 0;
		SDL_GetRendererOutputSize(context_.get_renderer().get_sdl_renderer(), &width, &height);

		ui_manager_->add_element(std::make_unique<engine::ui::UIPanel>(context_.get_renderer().get_logical_size(), glm::dvec2{ width,height },
			SDL_Color{64, 64, 64, 192}));

		glm::dvec2 position{width/2,height/2};
		glm::dvec2 offset;
		offset = { -48-96 ,0 };
		auto button_back = std::make_unique<engine::ui::UIButton>(context_, "Back1",
			"Back2", "Back3", position + offset);
		button_back->set_click(
			[this]()
			{
				engine::event::SwitchGameStateEvent event;
				event.action = "pop";
				context_.get_eventbus().publish(event);
			});

		offset = { 48,0 };
		auto button_quit = std::make_unique<engine::ui::UIButton>(context_, "Quit1",
			"Quit2", "Quit3", position + offset);
		button_quit->set_click(
			[this]()
			{
				engine::event::SwitchGameStateEvent event;
				event.state = engine::core::GameState::Title;
				event.action = "replace";
				context_.get_eventbus().publish(event);
			});

		button_back_ = button_back.get();
		ui_manager_->add_element(std::move(button_back));
		button_quit_ = button_quit.get();
		ui_manager_->add_element(std::move(button_quit));
	}
}