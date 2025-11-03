#include"game_scene.h"
#include"../core/wave_manager.h"
#include"../config/level_config.h"
#include"../object/object_manager.h"

#include"../../engine/core/context.h"
#include"../../engine/core/coordinator.h"
#include"../../engine/core/config.h"
#include"../../engine/render/renderer.h"
#include"../../engine/resource/resource_manager.h"
#include"../../engine/audio/audio_player.h"
#include"../../engine/core/event_bus.h"
#include"../../engine/core/input_manager.h"
#include"../../engine/ui/ui_manager.h"
#include"../../engine/ui/ui_button.h"
#include"../../engine/ui/ui_label.h"
#include"../../engine/ui/ui_image.h"
#include"../../engine/ui/ui_panel.h"
#include"../../engine/ui/ui_rounded_box.h"
#include"../../engine/ui/ui_round.h"
#include"../../engine/ui/ui_element.h"

#include"../../engine/system/animation_system.h"
#include"../../engine/system/remove_system.h"
#include"../../engine/system/rotate_system.h"
#include"../../engine/system/effect_system.h"
#include"../system/enemy_system.h"
#include"../system/aim_system.h"
#include"../system/fire_system.h"
#include"../system/player_system.h"

#include"../../engine/event/switch_game_state_event.h"
#include"../event/place_tower_event.h"
#include"../event/create_player_event.h"
#include"../event/enemy_enter_home_event.h"
#include"../event/player_mp_event.h"
#include"../event/enemy_hp_event.h"
#include"../event/reward_event.h"
#include"../object/enemy_type.h"

#include<spdlog/spdlog.h>

namespace game::scene
{
	GameScene::GameScene(Context& context)
		: Scene(context,"game scene")
	{}
	GameScene::~GameScene() = default;

	void GameScene::init()
	{
		Scene::init();
		init_config();
		init_system();
		init_ui();

		context_.get_audio_player().play_music("music_bgm");

		auto& event_bus = context_.get_eventbus();

		game::event::CreatePlayerEvent create_player_event;
		create_player_event.index = { 15,5 };
		event_bus.publish(create_player_event);
		
		subscribe_event<game::event::EnemyEnterHomeEvent>(
			[this,&event_bus](const game::event::EnemyEnterHomeEvent& event)
			{
				auto it = enemy_hp_bars_.find(event.entity);
				if (it != enemy_hp_bars_.end())
				{
					it->second.first->set_can_remove(true);
					it->second.second->set_can_remove(true);
					enemy_hp_bars_.erase(it);
				}

				if (--current_health_>=0)
					update_health_ui();
				if(current_health_ <= 0)
				{
					context_.is_game_win_ = false;
					engine::event::SwitchGameStateEvent switch_game_state_event;
					switch_game_state_event.state = engine::core::GameState::GameOver;
					switch_game_state_event.action = "push";
					event_bus.publish(switch_game_state_event);
					return;
				}
			}
		);
		
		subscribe_event<game::event::PlayerMpEvent>(
			[this](const game::event::PlayerMpEvent& event)
			{
				current_status_bar_process_ = event.process;
				update_status_bar_ui();
			});

		subscribe_event<game::event::EnemyHpEvent>(
			[this](const game::event::EnemyHpEvent& event)
			{
				auto it = enemy_hp_bars_.find(event.entity);
				if (event.process == 1 || event.process <= 0)
				{
					if(it==enemy_hp_bars_.end())
						return;
					it->second.first->set_can_remove(true);
					it->second.second->set_can_remove(true);
					enemy_hp_bars_.erase(it);
					return;
				}
				if (it == enemy_hp_bars_.end())
					create_enemy_ui(event.entity,event.position);
				else
					update_enemy_ui(event.entity, event.position, event.process);
			});

		subscribe_event<game::event::RewardEvent>(
			[this](const game::event::RewardEvent& event)
			{
				current_coin_ += event.reward;
				update_coin_ui();
			});
	}

	void GameScene::input(const InputEvent& event)
	{
		if (ui_manager_->input(context_, event))
			return;
		auto& input_manager = context_.get_input_manager();

		if (event.message == "MouseLeftClick"&&event.state==ActionState::PRESSED_THIS_FRAME)
		{
			if (tower_panel_)
			{
				tower_panel_->set_can_remove(true);
				tower_panel_ = nullptr;
				return;
			}
			auto mouse_position=input_manager.get_mouse_position();
			auto y = (int)mouse_position.y/game::config::SIZE_TILE;
			auto x = (int)mouse_position.x / game::config::SIZE_TILE;
			if (map_->can_place_tower(y, x))
			{
				create_tower_ui(y,x);
				return;
			}
		}
		else if (event.message == "pause") 
		{
			engine::event::SwitchGameStateEvent event;
			event.state = engine::core::GameState::Paused;
			event.action = "push";
			context_.get_eventbus().publish(event);
		}
	}

	void GameScene::update(double delta)
	{	
		object_manager_->update(delta);
		wave_manager_->update(delta);

		player_system_->update(delta, context_);//玩家状态更新
		
		fire_system_->update(delta, context_);//防御塔索敌开火
		aim_system_->update(delta, context_);//子弹瞄准敌人
		effect_system_->update(delta, context_);//特效攻击伤害计算
		enemy_system_->update(delta, context_,*wave_manager_);//敌人受击
		rotate_system_->update(delta, context_);//旋转
		remove_system_->update(delta, context_);//去除

		Scene::update(delta);
		animation_system_->update(delta, context_);//更新帧序列
	}

	void GameScene::clear()
	{
		Scene::clear();
		context_.get_audio_player().stop_music();
		context_.get_coordinator().clear();
	}

	bool GameScene::init_config()
	{
		//读取配置文件
		game::config::LevelConfig level_config;
		try
		{
			level_config.load_map("config/map.csv");
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化配置文件管理器失败:{}", e.what());
			return false;
		}
		try
		{
			level_config.load_level("config/level.json");
		}
		catch (const std::exception& e)
		{
			spdlog::error("初始化配置文件管理器失败:{}", e.what());
			return false;
		}
		level_config.parse_map(context_.get_resource_manager(), context_.get_renderer().get_sdl_renderer());

		texture_map_.reset(level_config.generate_map_texture());
		map_ = std::move(std::make_unique<game::core::Map>(level_config.generate_map()));

		object_manager_ = std::make_unique<ObjectManager>(context_);
		wave_manager_ = std::make_unique<WaveManager>(*object_manager_, context_.get_eventbus(),
			level_config.generate_wave(), level_config.generate_route());

		object_manager_->create_map(level_config, texture_map_.get());
		object_manager_->create_home(level_config);

		current_coin_ = level_config.get_initial_coin_num();
		current_health_ = context_.get_config().config_.home.hp;
		return true;
	}

	bool GameScene::init_system()
	{
		auto& coordinator = context_.get_coordinator();

		remove_system_ = coordinator.register_system<RemoveSystem>();
		rotate_system_ = coordinator.register_system<RotateSystem>();
		animation_system_ = coordinator.register_system<AnimationSystem>();
		enemy_system_ = coordinator.register_system<EnemySystem>();
		aim_system_ = coordinator.register_system<AimSystem>();
		fire_system_ = coordinator.register_system<FireSystem>();
		effect_system_ = coordinator.register_system<EffectSystem>();
		player_system_ = coordinator.register_system<PlayerSystem>();

		return remove_system_ && rotate_system_ && animation_system_ && enemy_system_ 
			&& aim_system_ &&fire_system_ && effect_system_ &&player_system_;
	}

	bool GameScene::init_ui()
	{
		if (!ui_manager_->init(context_.get_renderer().get_logical_size())) 
			return false;
		
		create_home_ui();
		create_health_ui();
		create_coin_ui();
		create_player_ui();
		return true;
	}

	void GameScene::create_home_ui()
	{
		auto& resource_manager = context_.get_resource_manager();
		auto texture = resource_manager.get_texture("ui_home");
		auto size = resource_manager.get_texture_size(texture);
		auto position = ui_manager_->get_root_element()->get_position() + glm::dvec2{ 15,15 };
		auto home_icon = std::make_unique<engine::ui::UIImage>(texture, position, size, SDL_Rect{ 0,0,size.x,size.y });
		home_icon_ = home_icon.get();

		ui_manager_->add_element(std::move(home_icon));
		
	}
	void GameScene::create_health_ui()
	{
		double spacing = 4.0;

		auto& resource_manager = context_.get_resource_manager();
		auto texture = resource_manager.get_texture("ui_heart");
		auto size = resource_manager.get_texture_size(texture);
		auto position = home_icon_->get_screen_position() + glm::dvec2{home_icon_->get_size().x+10, 0};

		auto health_panel = std::make_unique<engine::ui::UIPanel>(position);
		health_panel_ = health_panel.get();
		// --- 根据最大生命值，循环创建生命值图标(添加到UIPanel中) ---
		for (int i = 0; i < current_health_; ++i) 
		{          
			// 创建背景图标
			glm::dvec2 icon_position = { i * (size.x + spacing), 0 };
			auto bg_icon = std::make_unique<engine::ui::UIImage>(texture, icon_position, size,SDL_Rect{ 0,0,size.x,size.y });
			health_panel->add_child(std::move(bg_icon));
		}
		// 将UIPanel添加到UI管理器中
		ui_manager_->add_element(std::move(health_panel));
	}
	void GameScene::create_coin_ui()
	{
		auto& resource_manager = context_.get_resource_manager();
		auto texture = resource_manager.get_texture("ui_coin");
		auto size = resource_manager.get_texture_size(texture);
		auto position_icon = home_icon_->get_screen_position() + home_icon_->get_size() + glm::dvec2{ 10,-size.y };
		auto coin_icon = std::make_unique<engine::ui::UIImage>(texture, position_icon, size, SDL_Rect{ 0,0,size.x,size.y });

		ui_manager_->add_element(std::move(coin_icon));

		std::string coin_num_text = std::to_string(current_coin_);
		auto position_text = position_icon + glm::dvec2{ size.x + 10,0 };
		auto coin_text = std::make_unique<engine::ui::UILabel>(context_.get_text_renderer(), coin_num_text, "resources/font/ipix.ttf", 28, position_text);
		coin_label_ = coin_text.get();

		ui_manager_->add_element(std::move(coin_text));
	}
	void GameScene::create_player_ui()
	{
		auto& resource_manager = context_.get_resource_manager();
		auto texture = resource_manager.get_texture("ui_player");
		auto size = resource_manager.get_texture_size(texture);
		auto position = home_icon_->get_screen_position() + glm::dvec2{ 0 , home_icon_->get_size().y + 10};
		auto player_icon = std::make_unique<engine::ui::UIImage>(texture, position, size, SDL_Rect{ 0,0,size.x,size.y });

		ui_manager_->add_element(std::move(player_icon));

		auto size_status_bar_background = glm::dvec2{ width_mp_bar_+8,height_mp_bar_+8 };
		auto size_status_bar_foreground = glm::dvec2{ width_mp_bar_,height_mp_bar_ };
		auto position_status_bar_background = glm::dvec2{ health_panel_->get_screen_position().x,position.y };
		auto position_status_bar_foreground = position_status_bar_background + glm::dvec2{4,4};
		SDL_Color color_status_bar_background{ 48,40,51,255 };
		SDL_Color color_status_bar_foreground{ 144,121,173,255 };

		auto status_bar_background = std::make_unique<engine::ui::UIRounderBox>
			(position_status_bar_background,size_status_bar_background,color_status_bar_background,4);
		auto status_bar_foreground = std::make_unique<engine::ui::UIRounderBox>
			(position_status_bar_foreground, size_status_bar_foreground, color_status_bar_foreground,4);
		status_bar_foreground_ = status_bar_foreground.get();
		status_bar_background_ = status_bar_background.get();

		ui_manager_->add_element(std::move(status_bar_background));
		ui_manager_->add_element(std::move(status_bar_foreground));
	}
	void GameScene::update_coin_ui()
	{
		std::string coin_num_text = std::to_string(current_coin_);
		coin_label_->set_text(coin_num_text);
	}
	void GameScene::update_health_ui()
	{
		health_panel_->remove_child(health_panel_->get_children().back().get());
	}
	void GameScene::update_status_bar_ui()
	{
		status_bar_foreground_->set_size({ width_mp_bar_ *current_status_bar_process_,height_mp_bar_ });
	}
	void GameScene::create_enemy_ui(Entity entity, const glm::dvec2& position)
	{
		auto size = glm::dvec2{width_enemy_hp_bar_ , height_enemy_hp_bar_};
		auto enemy_hp_bar_position = position + glm::dvec2{ 0 - width_enemy_hp_bar_ / 2,-24 - height_enemy_hp_bar_ / 2 };
		const SDL_Color color_border{226,225,194,255 };
		const SDL_Color color_content{116,185,124,255 };

		auto enemy_hp_bar_border = std::make_unique<engine::ui::UIRounderBox>
			(enemy_hp_bar_position, size, color_border, 1);
		auto enemy_hp_bar_content= std::make_unique<engine::ui::UIRounderBox>
			(enemy_hp_bar_position, size, color_content, 1);
		enemy_hp_bars_[entity] = { enemy_hp_bar_border.get(),enemy_hp_bar_content.get()};

		ui_manager_->add_element(std::move(enemy_hp_bar_border));
		ui_manager_->add_element(std::move(enemy_hp_bar_content));
	}
	void GameScene::update_enemy_ui(Entity entity, const glm::dvec2& position, double process)
	{
		auto size = glm::dvec2{ width_enemy_hp_bar_*process , height_enemy_hp_bar_ };
		auto enemy_hp_bar_position = position + glm::dvec2{ 0- width_enemy_hp_bar_/2,-24- height_enemy_hp_bar_ /2 };
		enemy_hp_bars_[entity].second->set_size(size);
		enemy_hp_bars_[entity].first->set_position(enemy_hp_bar_position);
		enemy_hp_bars_[entity].second->set_position(enemy_hp_bar_position);
	}
	void GameScene::create_tower_ui(int y,int x)
	{
		auto tower_panel = std::make_unique<engine::ui::UIPanel>();
		tower_panel_ = tower_panel.get();
		ui_manager_->add_element(std::move(tower_panel));

		glm::dvec2 position
		{
			x * game::config::SIZE_TILE,
			y * game::config::SIZE_TILE
		}; 

		auto& resource_manager = context_.get_resource_manager();
		auto texture = resource_manager.get_texture("ui_select_cursor");
		auto size = resource_manager.get_texture_size(texture);
		auto ui_select_cursor=std::make_unique<engine::ui::UIImage>(texture,position,size,SDL_Rect{ 0,0,size.x,size.y });
		tower_panel_->add_child(std::move(ui_select_cursor));

		glm::dvec2 offset;
		offset = { 0 ,-64 };
		auto button_archer=std::make_unique<engine::ui::UIButton>(context_,"ui_place_archer_normal",
			"ui_place_archer_hover","ui_place_archer_hover",position + offset);
		offset = { -55.4256, 32 };
		auto button_axeman=std::make_unique<engine::ui::UIButton>(context_, "ui_place_axeman_normal",
			"ui_place_axeman_hover", "ui_place_axeman_hover", position + offset);
		offset = { 55.4256, 32 };
		auto button_gunner=std::make_unique<engine::ui::UIButton>(context_, "ui_place_gunner_normal",
			"ui_place_gunner_hover", "ui_place_gunner_hover", position + offset);
		
		auto tower_pos = position + glm::dvec2{ 24,24 };
		auto& config = context_.get_config().config_.towers;
		int cost = 0;
		cost = config["tower_archer"].cost;
		button_archer->set_click(
			[this,cost,y,x]()
			{
				tower_panel_->set_can_remove(true);
				try_place_tower(y,x, cost, TowerType::Archer);
			});
		button_archer->set_hover(
			[this, tower_pos,&config]()
			{
				double range = config["tower_archer"].view_range[0];
				int coin = config["tower_archer"].cost;
				tower_panel_->add_child(std::make_unique<engine::ui::UILabel>(context_.get_text_renderer(),
					std::to_string(coin),
					"resources/font/ipix.ttf",
					28, tower_pos + glm::dvec2{ -20,36 }));
				tower_panel_->add_child(std::make_unique<engine::ui::UIRound>(tower_pos,SDL_Color{ 0, 149, 217, 75 },range));
			});
		button_archer->set_unhover(
			[this]()
			{
				auto& v = tower_panel_->get_children();
				for (int i = 4; i < v.size(); ++i)
				{
					v[i]->set_can_remove(true);
				}
			});
		cost = config["tower_axeman"].cost;
		button_axeman->set_click(
			[this,cost, y,x]()
			{
				tower_panel_->set_can_remove(true);
				try_place_tower(y,x, cost, TowerType::Axeman);
			});
		button_axeman->set_hover(
			[this, tower_pos, &config]()
			{
				double range = config["tower_axeman"].view_range[0];
				int coin= config["tower_axeman"].cost;
				tower_panel_->add_child(std::make_unique<engine::ui::UILabel>(context_.get_text_renderer(),
					std::to_string(coin),
					"resources/font/ipix.ttf",
					28,tower_pos+glm::dvec2{ -20,36 }));
				tower_panel_->add_child(std::make_unique<engine::ui::UIRound>(tower_pos, SDL_Color{ 0, 149, 217, 75 }, range));
			});
		button_axeman->set_unhover(
			[this]()
			{
				auto& v = tower_panel_->get_children();
				for (int i = 4; i < v.size(); ++i)
				{
					v[i]->set_can_remove(true);
				}
			});
		cost = config["tower_gunner"].cost;
		button_gunner->set_click(
			[this,cost, y,x]()
			{
				tower_panel_->set_can_remove(true);
				try_place_tower(y,x, cost, TowerType::Gunner);
			});
		button_gunner->set_hover(
			[this, tower_pos, &config]()
			{
				double range = config["tower_gunner"].view_range[0];
				int coin = config["tower_gunner"].cost;
				tower_panel_->add_child(std::make_unique<engine::ui::UILabel>(context_.get_text_renderer(),
					std::to_string(coin),
					"resources/font/ipix.ttf",
					28, tower_pos + glm::dvec2{ -20,36 }));
				tower_panel_->add_child(std::make_unique<engine::ui::UIRound>(tower_pos, SDL_Color{ 0, 149, 217, 75 }, range));
			});
		button_gunner->set_unhover(
			[this]()
			{
				auto& v = tower_panel_->get_children();
				for (int i = 4; i < v.size(); ++i)
				{
					v[i]->set_can_remove(true);
				}
			});

		tower_panel_->add_child(std::move(button_archer));
		tower_panel_->add_child(std::move(button_axeman));
		tower_panel_->add_child(std::move(button_gunner));
	}
	void GameScene::try_place_tower(int y,int x, int cost, TowerType type)
	{
		if (cost > current_coin_)
			return;
		current_coin_ -= cost;
		map_->place_tower(y,x);
		update_coin_ui();

		auto& event_bus = context_.get_eventbus();
		game::event::PlaceTowerEvent event;
		event.type = type;
		event.index = {x,y};
		event_bus.publish(event);
	}
}