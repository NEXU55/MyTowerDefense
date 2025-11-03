#pragma once
#include"animation_clip.h"
#include<nlohmann/json_fwd.hpp>
#include<SDL_rect.h>
#include<spdlog/spdlog.h>
#include<unordered_map>
#include<string_view>


namespace engine::resource
{
	class TextureManager;

	class AnimationManager
	{
		using json = nlohmann::json;
	private:
		std::unordered_map<std::string,AnimationClip> animation_pool_;
	public:
		void load(const std::string&, TextureManager&);

		AnimationClip* get(std::string_view path)
		{
			auto it = animation_pool_.find(std::string(path));
			if (it == animation_pool_.end())
			{
				spdlog::error("未能找到该动画字段，{}",path);
				return nullptr;
			}
			return &(it->second);
		}

		void clear()
		{
			animation_pool_.clear();
		}
	private:
		void set_animation_frame(std::vector<SDL_Rect>&,SDL_Rect,int,int,const std::vector<int>&);
	};
}