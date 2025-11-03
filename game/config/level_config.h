#pragma once
#include"tile.h"
#include"wave.h"
#include"../core/map.h"
#include<unordered_map>
#include<vector>
#include<string_view>
#include<string>
#include<SDL.h>

namespace engine::resource
{
	class ResourceManager;
}

namespace game::config
{
	//用于单次加载关卡需要的配置信息，加载完毕后销毁
	class LevelConfig
	{
		using ResourceManager=engine::resource:: ResourceManager;

		using TileMap = std::vector<std::vector<Tile>>;
		using Route = std::vector<SDL_Point>;
		using RoutePool = std::unordered_map<int, Route>;
		using WaveList = std::vector<Wave>;
	private:
		TileMap tilemap_;						//存储瓦片地图渲染信息
		game::core::Map map_;					//地图运行时信息
		SDL_Texture* texture_map_=nullptr;		//存储地图纹理
		RoutePool route_pool_;					//存储敌人洋流图
		SDL_Point index_home_ = { 0 };
		WaveList wave_list_;					//存储敌人波次
		int initial_coin_num_ = 0;				//存储金币数
	public:
		void load_map(std::string_view);		//读取地图配置信息
		void load_level(std::string_view);		//读取关卡配置信息
		void parse_map(ResourceManager&, SDL_Renderer*);//解析地图，生成地图纹理，洋流图和地图组件的字段

		game::core::Map&& generate_map() { return std::move(map_); }
		SDL_Texture* generate_map_texture() { return texture_map_; }	//形成地图纹理，用于主场景
		RoutePool&& generate_route() { return std::move(route_pool_); }	//形成洋流图，用于洋流图管理器
		WaveList&& generate_wave() { return std::move(wave_list_); }	//形成敌人波次，用于波次管理器

		size_t get_tilemap_height()const { return tilemap_.size(); }
		size_t get_tilemap_width()const { return tilemap_.empty() ? 0 : tilemap_[0].size(); }
		SDL_Point get_index_home()const { return index_home_; }
		int get_initial_coin_num()const { return initial_coin_num_; }
	private:
		std::vector<std::string> split(const std::string&,char);//解析单个字符串
		Route generate_route(SDL_Point);//解析地图，生成一条路径
		bool check_duplicate_index(const Route&,SDL_Point);//判断洋流图上有无重复路径
	};
}