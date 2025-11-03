#include"title_scene.h"

#include"../../engine/core/context.h"
#include"../../engine/core/event_bus.h"
#include"../../engine/render/renderer.h"
#include"../../engine/resource/resource_manager.h"

#include"../../engine/ui/ui_manager.h"
#include"../../engine/ui/ui_button.h"
#include"../../engine/ui/ui_image.h"

#include"../../engine/event/switch_game_state_event.h"
#include"../event/game_quit_event.h"

#include<spdlog/spdlog.h>

namespace game::scene
{
	TitleScene::TitleScene(Context& context)
		: Scene(context, "title scene")
	{
	}
	TitleScene::~TitleScene() = default;

	void TitleScene::init()
	{
		Scene::init();
		create_ui();
	}

	void TitleScene::create_ui()
	{
		auto& resource_manager = context_.get_resource_manager();
		auto texture = resource_manager.get_texture("back");
		auto size = resource_manager.get_texture_size(texture);

		int width = 0, height = 0;
		SDL_GetRendererOutputSize(context_.get_renderer().get_sdl_renderer(), &width, &height);

		glm::dvec2 position{ width / 2,height / 2 };
		glm::dvec2 offset;
		offset = { -48 - 96 ,0 };
		auto button_start = std::make_unique<engine::ui::UIButton>(context_, "Start1",
			"Start2", "Start3", position + offset);
		button_start->set_click(
			[this]()
			{
				engine::event::SwitchGameStateEvent event;
				event.action = "replace";
				event.state = engine::core::GameState::Playing;
				context_.get_eventbus().publish(event);
			});

		offset = { 48,0 };
		auto button_quit = std::make_unique<engine::ui::UIButton>(context_, "Quit1",
			"Quit2", "Quit3", position + offset);
		button_quit->set_click(
			[this]()
			{
				engine::event::SwitchGameStateEvent event;
				event.action = "pop";
				context_.get_eventbus().publish(event);

				game::event::GameQuitEvent game_quit_event;
				context_.get_eventbus().publish(game_quit_event);
			});

		button_start_ = button_start.get();
		ui_manager_->add_element(std::move(button_start));
		button_quit_ = button_quit.get();
		ui_manager_->add_element(std::move(button_quit));
	}
}