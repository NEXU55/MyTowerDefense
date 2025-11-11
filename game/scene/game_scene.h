#pragma once
#include"../../engine/scene/scene.h"
#include"../../engine/core/type.h"
#include"../object/tower_type.h"
#include<glm/vec2.hpp>
#include<unordered_map>
#include<memory>
#include <SDL.h>

namespace engine::system
{
	class AnimationSystem;
	class RemoveSystem;
	class RotateSystem;
	class EffectSystem;
	class PhysicsSystem;
	class CollisionSystem;
}
namespace engine::ui
{
	class UILabel;
	class UIImage;
	class UIPanel;
	class UIRounderBox;
}
namespace game::core
{
	class Map;
	class WaveManager;
}
namespace game::object
{
	class ObjectManager;
}
namespace game::system
{
	class EnemySystem;
	class AimSystem;
	class FireSystem;
	class PlayerSystem;
}

namespace game::scene
{
	class GameScene final:public engine::scene::Scene
	{
		using WaveManager=game::core::WaveManager;
		using ObjectManager = game::object::ObjectManager;

		using AnimationSystem = engine::system::AnimationSystem;
		using EnemySystem = game::system::EnemySystem;
		using AimSystem = game::system::AimSystem;
		using FireSystem = game::system::FireSystem;
		using RemoveSystem = engine::system::RemoveSystem;
		using RotateSystem = engine::system::RotateSystem;
		using EffectSystem = engine::system::EffectSystem;
		using PlayerSystem = game::system::PlayerSystem;
		using PhysicsSystem = engine::system::PhysicsSystem;
		using CollisionSystem = engine::system::CollisionSystem;

		struct SDLTextureDeleter
		{
			void operator()(SDL_Texture* texture)
			{
				if (texture)
					SDL_DestroyTexture(texture);
			}
		};
	private:
		int current_coin_ = 0;
		int current_health_ = 0;
		double current_status_bar_process_ = 0;
		//ui
		const int width_mp_bar_ = 200;
		const int height_mp_bar_ = 20;
		const int width_enemy_hp_bar_ = 30;
		const int height_enemy_hp_bar_ = 4;
		engine::ui::UIImage* home_icon_ = nullptr;					//家ui，用于其他ui确定相对位置
		engine::ui::UIPanel* health_panel_ = nullptr;				//生命值ui，用于更新生命值
		engine::ui::UIRounderBox* status_bar_foreground_ = nullptr; //玩家能量条ui,用于更新能量条
		engine::ui::UIRounderBox* status_bar_background_ = nullptr; //玩家能量条ui,用于更新能量条
		
		engine::ui::UIPanel* tower_panel_ = nullptr;				//放置防御塔轮盘ui，用于放置防御塔
		engine::ui::UILabel* coin_label_ = nullptr;					//金币数量ui，用于更新金币数量
		engine::ui::UILabel* fps_label_ = nullptr;			
		engine::ui::UILabel* enemy_label_ = nullptr;

		std::unordered_map<Entity,engine::ui::UIPanel*> enemy_hp_bars_;//敌人血量条ui,用于更新敌人血量

		//运行时
		std::unique_ptr<SDL_Texture, SDLTextureDeleter>  texture_map_;	//地图纹理
		std::unique_ptr<game::core::Map> map_;							//地图信息

		std::unique_ptr<WaveManager> wave_manager_;						//敌人波次管理器
		std::unique_ptr<ObjectManager> object_manager_;					//生成管理器

		RemoveSystem* remove_system_ = nullptr;
		RotateSystem* rotate_system_ = nullptr;
		AnimationSystem* animation_system_ = nullptr;
		EnemySystem* enemy_system_ = nullptr;
		AimSystem* aim_system_ = nullptr;
		FireSystem* fire_system_ = nullptr;
		EffectSystem* effect_system_ = nullptr;
		PlayerSystem* player_system_ = nullptr;
		PhysicsSystem* physics_system_ = nullptr;
		CollisionSystem* collision_system_ = nullptr;

		int fps_ = 0;
		double second_ = 0;
	public:
		GameScene(Context&);
		~GameScene()override;
		void init()override;
		void input(const InputEvent& event)override;
		void update(double)override;
		void clear()override;
	private:
		bool init_config();
		bool init_system();
		bool init_ui();

		void create_home_ui();
		void create_health_ui();
		void create_coin_ui();
		void create_player_ui();
		void create_fps_ui();

		void update_coin_ui();
		void update_health_ui();
		void update_status_bar_ui();
		void update_fps_ui(double);

		void create_enemy_ui(Entity entity,const glm::dvec2& position);
		void update_enemy_ui();

		void create_tower_ui(int y,int x);

		void try_place_tower(int y,int x,int cost,TowerType type);
	};
}