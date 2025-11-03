#include"end_scene.h"

#include"../../engine/core/context.h"
#include"../../engine/render/renderer.h"
#include"../../engine/resource/resource_manager.h"

#include"../../engine/ui/ui_manager.h"
#include"../../engine/ui/ui_image.h"
#include"../../engine/ui/ui_panel.h"
#include"../../engine/ui/ui_element.h"

#include"../../engine/event/play_sound_event.h"
#include"../../engine/event/switch_game_state_event.h"

#include<spdlog/spdlog.h>

namespace game::scene
{
	EndScene::EndScene(Context& context)
		: Scene(context, "end scene")
	{
	}
	EndScene::~EndScene() = default;

	void EndScene::init()
	{
		Scene::init();
		is_win_ = context_.is_game_win_;
		create_ui();

		engine::event::PlaySoundEventEvent play_sound_event;
		play_sound_event.type = "game";
		if (is_win_)
			play_sound_event.name = "win";
		else
			play_sound_event.name = "loss";
		context_.get_eventbus().publish(play_sound_event);

	}

	void EndScene::update(double delta)
	{
		auto position = icon_->get_position() + glm::dvec2{ speed_ * delta,0 };
		auto size = icon_->get_size();
		int width = 0, height = 0;
		SDL_GetRendererOutputSize(context_.get_renderer().get_sdl_renderer(), &width, &height);
		auto final_position = ui_manager_->get_root_element()->get_position() + glm::dvec2{ width / 2 - size.x /2,height / 2 - size.y / 2 };
		if (position.x >= final_position.x)
			position = final_position;
		icon_->set_position(position);
	}

	void EndScene::input(const InputEvent& event)
	{
		if (event.message == "pause")
		{
			engine::event::SwitchGameStateEvent event;
			event.state = engine::core::GameState::Title;
			event.action = "replace";
			context_.get_eventbus().publish(event);
		}
	}

	void EndScene::create_ui()
	{
		auto& resource_manager = context_.get_resource_manager();
		int width = 0, height = 0;
		SDL_GetRendererOutputSize(context_.get_renderer().get_sdl_renderer(), &width, &height);

		auto texture_bar = resource_manager.get_texture("ui_game_over_bar");
		auto size_bar = resource_manager.get_texture_size(texture_bar);
		auto position_bar = ui_manager_->get_root_element()->get_position() + glm::dvec2{ width / 2 -size_bar.x/2,height / 2 - size_bar.y / 2 };
		auto bar = std::make_unique<engine::ui::UIImage>(texture_bar,position_bar, size_bar,SDL_Rect{ 0,0,size_bar.x,size_bar.y });
		ui_manager_->add_element(std::move(bar));

		std::string name = is_win_ ? "ui_win_text" : "ui_loss_text";
		auto texture = resource_manager.get_texture(name);
		auto size = resource_manager.get_texture_size(texture);
		auto position = ui_manager_->get_root_element()->get_position() + glm::dvec2{ width / 2 - size.x * 1.5,height / 2 - size.y / 2 };
		auto icon = std::make_unique<engine::ui::UIImage>(texture, position, size, SDL_Rect{ 0,0,size.x,size.y });
		icon_ = icon.get();
		ui_manager_->add_element(std::move(icon));
	}
}