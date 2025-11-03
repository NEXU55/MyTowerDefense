#include"game.h"
#include"../../engine/core/event_bus.h"
#include"../../engine/core/config.h"
#include"../../engine/core/sdl_manager.h"
#include"../../engine/core/input_manager.h"
#include"../../engine/core/time.h"
#include"../../engine/resource/resource_manager.h"
#include"../../engine/render/renderer.h"
#include"../../engine/render/text_renderer.h"
#include"../../engine/audio/audio_player.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/context.h"
#include"../../engine/scene/scene_manager.h"
#include"register.h"
#include"../scene/game_scene.h"
#include"../scene/end_scene.h"
#include"../scene/pause_scene.h"
#include"../scene/title_scene.h"

#include"../../engine/event/switch_game_state_event.h"
#include"../../game/event/game_quit_event.h"
#include <spdlog/spdlog.h>

namespace game::core
{
	Game::Game() = default;
	Game::~Game()
	{
		if (is_running_)
		{
			spdlog::warn("游戏还未关闭,正在关闭");
			close();
		}
	}
	void Game::run()
	{
		if (!init())
		{
			spdlog::error("初始化失败");
			return;
		}

		auto game_quit_event_ptr = eventbus_->subscribe<game::event::GameQuitEvent>(
			[this](const game::event::GameQuitEvent& event) 
			{
				is_running_ = false;
			});

		auto switch_event_ptr = eventbus_->subscribe<engine::event::SwitchGameStateEvent>(
			[this](const engine::event::SwitchGameStateEvent & event)
		{
			game_state_ = event.state;
			std::unique_ptr<engine::scene::Scene> scene;
			switch (game_state_)
			{
			case engine::core::GameState::Playing:
				scene = std::move(std::make_unique<game::scene::GameScene>(*context_));
				break;
			case engine::core::GameState::GameOver:
				scene = std::move(std::make_unique<game::scene::EndScene>(*context_));
				break;
			case engine::core::GameState::Paused:
				scene = std::move(std::make_unique<game::scene::PauseScene>(*context_));
				break;
			case engine::core::GameState::Title:
				scene = std::move(std::make_unique<game::scene::TitleScene>(*context_));
				break;
			}
			if (event.action == "pop")
				scene_manager_->request_pop();
			else if (!scene)
				return;
			else if (event.action == "push")
				scene_manager_->request_push(std::move(scene));
			else if (event.action == "replace")
				scene_manager_->request_replace(std::move(scene));
		}); 

		engine::event::SwitchGameStateEvent switch_game_state_event;
		switch_game_state_event.state = engine::core::GameState::Title;
		switch_game_state_event.action = "push";
		eventbus_->publish(switch_game_state_event);

		while (is_running_)
		{
			time_->on_update();
			double delta = time_->get_delta();
			input();
			update(delta);
			render();
		}
		close();
	}
	bool Game::init()
	{
		spdlog::set_level(spdlog::level::trace);
		spdlog::trace("正在初始化游戏");
		//对事件总线进行初始化
		if (!init_eventbus())
		return false;
		//对配置文件管理器进行初始化
		if (!init_config())
			return false;
		//对SDL进行初始化并创建窗口和渲染器
		if (!init_SDL())
			return false;
		//对输入管理器进行初始化
		if (!init_input_manager())
			return false;
		//对时间进行初始化
		if (!init_time())
			return false;
		//对资源管理器进行初始化
		if (!init_resource_manager())
			return false;
		//对渲染器进行初始化
		if (!init_renderer())
			return false;
		//对文本渲染器进行初始化
		if (!init_text_renderer())
			return false;
		//对音乐播放器进行初始化
		if(!init_audio_player())
			return false;
		//对协调器进行初始化
		if (!init_coordinator())
			return false;
		//对上下文进行初始化
		if (!init_context())
			return false;
		//对场景管理器进行初始化
		if (!init_scene_manager())
			return false;

		register_->register_all(*context_);
		is_running_ = true;
		return true;
	}
	bool Game::init_config()
	{
		try
		{
			config_manager_ = std::make_unique<ConfigManager>("config/config.json");
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化配置文件管理器失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_SDL()
	{
		try
		{
			sdl_manager_ = std::make_unique<SDLManager>();
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化SDL失败:{}", e.what());
			return false;
		}

		sdl_window_.reset(SDL_CreateWindow("村庄保卫战"
			, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
			, config_manager_->config_.basic.window.window_width
			,config_manager_->config_.basic.window.window_height, SDL_WINDOW_SHOWN));
		if (!sdl_window_)
		{
			spdlog::error("无法创建SDL窗口：{}", SDL_GetError());
			return false;
		}

		sdl_renderer_.reset(SDL_CreateRenderer(sdl_window_.get(), -1
			, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE));
		if (!sdl_renderer_)
		{
			spdlog::error("无法创建SDL渲染器：{}", SDL_GetError());
			return false;
		}
		SDL_SetRenderDrawBlendMode(sdl_renderer_.get(), SDL_BLENDMODE_BLEND);

		return true;
	}
	bool Game::init_time()
	{
		try
		{
			time_ = std::make_unique<Time>();
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化时间管理器失败:{}", e.what());
			return false;
		}
		time_->set_target_fps(config_manager_->config_.basic.performance.target_fps);
		return true;
	}
	bool Game::init_resource_manager()
	{
		try
		{
			resource_manager_ = std::make_unique<ResourceManager>(sdl_renderer_.get());
			resource_manager_->load_all();
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化资源管理器失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_renderer()
	{
		try
		{
			renderer_ = std::make_unique<Renderer>(sdl_renderer_.get(),resource_manager_.get());
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化渲染器失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_text_renderer()
	{
		try
		{
			text_renderer_ = std::make_unique<TextRenderer>(sdl_renderer_.get(), resource_manager_.get());
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化文本渲染器失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_audio_player()
	{
		try
		{
			audio_player_ = std::make_unique<AudioPlayer>(resource_manager_.get());
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化音乐播放器失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_coordinator()
	{
		try
		{
			coordinator_ = std::make_unique<Coordinator>();
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化协调器失败:{}", e.what());
			return false;
		}
		register_ = std::make_unique<Register>();

		return true;
	}
	bool Game::init_eventbus()
	{
		try
		{
			eventbus_ = std::make_unique<EventBus>();
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化事件总线失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_input_manager()
	{
		try
		{
			input_manager_ = std::make_unique<InputManager>(*eventbus_,config_manager_->config_.basic.input_list);
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化输入管理器失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_context()
	{
		try
		{
			context_ = std::make_unique<Context>(*eventbus_,*config_manager_,*input_manager_,
				*resource_manager_,*text_renderer_,*renderer_,*audio_player_,*coordinator_,game_state_);
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化上下文管理器失败:{}", e.what());
			return false;
		}
		return true;
	}
	bool Game::init_scene_manager()
	{
		try
		{
			scene_manager_ = std::make_unique<SceneManager>(*context_);
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化场景管理器失败:{}", e.what());
			return false;
		}
		return true;
	}
	void Game::close()
	{
		spdlog::trace("游戏关闭");
		scene_manager_.reset();
		context_.reset();
		coordinator_.reset();
		text_renderer_.reset();
		renderer_.reset();
		resource_manager_.reset();
		time_.reset();
		input_manager_.reset();
		sdl_renderer_.reset();
		sdl_window_.reset();
		sdl_manager_.reset();
		config_manager_.reset();
		eventbus_.reset();
		is_running_ = false;
	}
	void Game::input()
	{
		input_manager_->update();
		if (input_manager_->get_is_running() == false)
			is_running_ = false;
	}
	void Game::update(double delta)
	{
		register_->update(delta,*context_);
		scene_manager_->update(delta);
	}
	void Game::render()
	{
		SDL_SetRenderDrawColor(sdl_renderer_.get(), 0, 0, 0, 255);
		renderer_->clear_screen();

		register_->render(*context_);
		scene_manager_->render();

		renderer_->present();
	}
}

