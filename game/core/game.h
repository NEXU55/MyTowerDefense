#pragma once
#include"../../engine/core/game_state.h"
#include<memory>
#include<SDL.h>

namespace engine::core
{
	class EventBus;
	class ConfigManager;
	class SDLManager;
	class InputManager;
	class Time;
	class Coordinator;
	class Context;
}
namespace engine::resource 
{
	class ResourceManager;
}
namespace engine::render
{
	class Renderer;
	class TextRenderer;
}
namespace engine::audio
{
	class AudioPlayer;
}
namespace engine::scene
{
	class SceneManager;
}
namespace game::core
{
	class Register;
	struct WindowDeleter
	{
		void operator()(SDL_Window* window)
		{
			SDL_DestroyWindow(window);
		}
	};
	struct RendererDeleter
	{
		void operator()(SDL_Renderer* renderer)
		{
			SDL_DestroyRenderer(renderer);
		}
	};

	class Game final
	{
		using GameState = engine::core::GameState;
		using EventBus = engine::core::EventBus;
		using ConfigManager = engine::core::ConfigManager;
		using SDLManager=engine::core::SDLManager;
		using Time = engine::core::Time;
		using InputManager = engine::core::InputManager;
		using ResourceManager = engine::resource::ResourceManager;
		using TextRenderer = engine::render::TextRenderer;
		using Renderer = engine::render::Renderer;
		using AudioPlayer = engine::audio::AudioPlayer;
		using Coordinator = engine::core::Coordinator;
		using Context = engine::core::Context;
		using SceneManager = engine::scene::SceneManager;
	public:
		Game();
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;

		~Game();

		void run();
	private:
		bool init();
		bool init_eventbus();
		bool init_config();
		bool init_SDL();
		bool init_input_manager();
		bool init_time();
		bool init_resource_manager();
		bool init_renderer();
		bool init_text_renderer();
		bool init_audio_player();
		bool init_coordinator();
		bool init_context();
		bool init_scene_manager();

		void close();

		void input();
		void update(double delta);
		void render();
	private:
		bool is_running_ = false;

		GameState game_state_ = GameState::Playing;
		std::unique_ptr<EventBus> eventbus_;
		std::unique_ptr<ConfigManager> config_manager_;
		std::unique_ptr<SDL_Window,WindowDeleter> sdl_window_;
		std::unique_ptr<SDL_Renderer,RendererDeleter> sdl_renderer_;
		std::unique_ptr<SDLManager> sdl_manager_;
		std::unique_ptr<InputManager> input_manager_;
		std::unique_ptr<Time> time_;
		std::unique_ptr<ResourceManager> resource_manager_;
		std::unique_ptr<Renderer> renderer_;
		std::unique_ptr<TextRenderer> text_renderer_;
		std::unique_ptr<AudioPlayer> audio_player_;
		std::unique_ptr<Coordinator> coordinator_;
		std::unique_ptr<Context> context_;
		std::unique_ptr<SceneManager> scene_manager_;
		std::unique_ptr<Register> register_;
	};
}