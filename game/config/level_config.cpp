#include"level_config.h"
#include"../../engine/resource/resource_manager.h"
#include"nlohmann/json.hpp"
#include<spdlog/spdlog.h>
#include<SDL_image.h>
#include<fstream>


namespace game::config
{
	void LevelConfig::load_map(std::string_view path)
	{
		std::ifstream file(path.data());
		if (!file.good())
		{
			spdlog::warn("地图配置文件 '{}' 未找到。", path);
			throw std::runtime_error("Failed to open config file: " + std::string(path));
		}
		TileMap tilemap;
		std::string str_line;
		while (std::getline(file, str_line))
		{
			std::vector<Tile> row;
			const auto& str_tile_list = split(str_line, ',');
			for (const auto& str_tile : str_tile_list)
			{
				const auto& parts = split(str_tile, '\\');
				if (parts.size() != 4)
				{
					row.emplace_back();
					continue;
				}
				Tile tile;
				int dir = 0;
				tile.terrain = std::stoi(parts[0]);
				tile.decoration = std::stoi(parts[1]);
				dir = std::stoi(parts[2]);
				tile.special_flag = std::stoi(parts[3]);
				if (dir >= 0 && dir <= 4)
					tile.dir = static_cast<Tile::Direction>(dir);
				row.emplace_back(tile);
			}
			tilemap.emplace_back(std::move(row));
		}
		if (tilemap.empty() || tilemap[0].empty())
		{
			spdlog::warn("无法解析地图配置文件{}", path);
			throw std::runtime_error("Failed to parse config file: " + std::string(path));
		}
		tilemap_ = std::move(tilemap);
	}

	void LevelConfig::load_level(std::string_view path)
	{
		std::ifstream file(path.data());
		if (!file.good())
		{
			spdlog::warn("关卡配置文件 '{}' 未找到。", path);
			throw std::runtime_error("Failed to open config file: " + std::string(path));
		}
		nlohmann::json json_root;
		file >> json_root;

		json_root.at("initial_coin_num").get_to(initial_coin_num_);

		const auto& json_waves = json_root.at("wave");
		if (!json_waves.is_array())
		{
			spdlog::warn("关卡配置文件 '{}' 格式不正确，必须是数组形式。", path);
			throw std::runtime_error("Failed to parse config file: " + std::string(path));
		}

		for (const auto& json_wave : json_waves)
		{
			Wave wave;
			json_wave.at("interval").get_to(wave.interval);
			json_wave.at("rewards").get_to(wave.rewards);

			const auto& json_spawn_array = json_wave["spawn_list"];
			if (!json_spawn_array.is_array())
				continue;
			for (const auto& json_spawn : json_spawn_array)
			{
				Wave::SpawnEvent spawn_event;
				json_spawn.at("interval").get_to(spawn_event.interval);
				json_spawn.at("point").get_to(spawn_event.spawn_point);
				std::string str_enemy;
				json_spawn.at("enemy").get_to(str_enemy);
				if (str_enemy == "Slime")
					spawn_event.enemy_type = EnemyType::Slime;
				else if (str_enemy == "RoyalSlime")
					spawn_event.enemy_type = EnemyType::RoyalSlime;
				else if (str_enemy == "Skeleton")
					spawn_event.enemy_type = EnemyType::Skeleton;
				else if (str_enemy == "Goblin")
					spawn_event.enemy_type = EnemyType::Goblin;
				else if (str_enemy == "GoblinPriest")
					spawn_event.enemy_type = EnemyType::GoblinPriest;

				wave.spawn_event_list.emplace_back(std::move(spawn_event));
			}
			if (!wave.spawn_event_list.empty())
				wave_list_.emplace_back(std::move(wave));
		}
		if (wave_list_.empty())
		{
			spdlog::warn("生成的敌人波次列表为空");
			throw std::runtime_error("Failed to parse config file: " + std::string(path));
		}
	}

	std::vector<std::string> LevelConfig::split(const std::string& s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.emplace_back(token);
		}
		return tokens;
	}

	LevelConfig::Route LevelConfig::generate_route(SDL_Point index_origin)
	{
		Route route;
		auto index_next = index_origin;
		while (true)
		{
			auto& [x, y] = index_next;
			if (x >= get_tilemap_width() || y >= get_tilemap_height() || x < 0 || y < 0)
				break;
			if (check_duplicate_index(route,index_next))
				break;

			route.emplace_back(index_next);
			bool is_next_dir_exist = true;
			const auto& tile = tilemap_[y][x];

			if (tile.special_flag == 0)
				break;

			switch (tile.dir)
			{
			case Tile::Direction::Up:
				--y;
				break;
			case Tile::Direction::Down:
				++y;
				break;
			case Tile::Direction::Left:
				--x;
				break;
			case Tile::Direction::Right:
				++x;
				break;
			default:
				is_next_dir_exist = false;
				break;
			}
			if (!is_next_dir_exist)
				break;
		}
		return route;
	}

	bool LevelConfig::check_duplicate_index(const Route& route,SDL_Point index)
	{
		{
			for (const auto& [x, y] : route)
			{
				if (x == index.x && y == index.y)
					return true;
			}
			return false;
		}
	}
	
	void LevelConfig::parse_map(ResourceManager& resource_manager,SDL_Renderer* renderer)
	{
		map_ = game::core::Map(get_tilemap_height(), get_tilemap_width());

		//生成家园和敌人洋流图
		for (int y = 0; y < get_tilemap_height(); ++y)
		{
			for (int x = 0; x < get_tilemap_width(); ++x)
			{
				const Tile& tile = tilemap_[y][x];
				if (tile.special_flag == 0)
					index_home_ = { x,y };
				else if (tile.special_flag > 0)
					route_pool_[tile.special_flag] = generate_route({ x,y });
				else
					map_.map_[y][x].can_place_tower = tile.decoration < 0 && tile.dir == Tile::Direction::None;
			}
		}
		//生成地图纹理
		SDL_Texture* tileset = resource_manager.get_texture("tileset");
		auto tilset_size=resource_manager.get_texture_size(tileset);
		int num_tile_single_line = tilset_size.x / SIZE_TILE;

		size_t texture_width_ = 0;
		size_t texture_height_ = 0;
		texture_width_= get_tilemap_width() * SIZE_TILE;
		texture_height_ = get_tilemap_height() * SIZE_TILE;
		texture_map_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888
			, SDL_TEXTUREACCESS_TARGET, texture_width_, texture_height_);
		if (!texture_map_)
		{
			spdlog::warn("创建地图纹理失败");
			throw std::runtime_error("Failed to create map texture: ");
		}

		SDL_SetTextureBlendMode(texture_map_, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, texture_map_);

		for (int y = 0; y < get_tilemap_height(); ++y)
		{
			for (int x = 0; x < get_tilemap_width(); ++x)
			{
				const Tile& tile = tilemap_[y][x];
				SDL_Rect rect_src =
				{
					(tile.terrain % num_tile_single_line) * SIZE_TILE,
					(tile.terrain / num_tile_single_line) * SIZE_TILE,
					SIZE_TILE,
					SIZE_TILE
				};
				SDL_Rect rect_dst =
				{
					x * SIZE_TILE,
					y * SIZE_TILE,
					SIZE_TILE,
					SIZE_TILE
				};
				SDL_RenderCopy(renderer, tileset, &rect_src, &rect_dst);

				if (tile.decoration >= 0)
				{
					rect_src =
					{
						(tile.decoration % num_tile_single_line) * SIZE_TILE,
						(tile.decoration / num_tile_single_line) * SIZE_TILE,
						SIZE_TILE,
						SIZE_TILE
					};
					SDL_RenderCopy(renderer, tileset, &rect_src, &rect_dst);
				}
			}
		}
		SDL_SetRenderTarget(renderer, nullptr);
	}
}