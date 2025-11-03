#pragma once

namespace engine::core
{
	class SDLManager
	{
	public:
		SDLManager();
		SDLManager(const SDLManager&) = delete;
		SDLManager(SDLManager&&) = delete;
		SDLManager& operator=(const SDLManager&) = delete;
		SDLManager& operator=(SDLManager&&) = delete;

		~SDLManager();
	};
};