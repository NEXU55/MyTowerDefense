#include "sdl_manager.h"
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>
#include<spdlog/spdlog.h>

namespace engine::core
{
	SDLManager::SDLManager()
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0)
			throw std::runtime_error("SDL初始化失败");
		if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
			throw std::runtime_error("SDL_IMG初始化失败");
		if (TTF_Init() != 0)
			throw std::runtime_error("SDL_TTF初始化失败");
		if (Mix_Init(MIX_INIT_MP3) == 0)
			throw std::runtime_error("SDL_Mix初始化失败");
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
			throw std::runtime_error("SDL_Mix初始化失败");
	}
	SDLManager::~SDLManager()
	{
		Mix_CloseAudio();
		Mix_Quit();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	}
}
