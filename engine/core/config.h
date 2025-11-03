#pragma once
#include <string_view>
#include <vector>
#include <unordered_map>
#include<nlohmann/json_fwd.hpp>

namespace engine::core 
{
    namespace config
    {
        using json = nlohmann::json;
        // 定义配置数据结构
        // 窗口设置
        struct WindowConfig
        {
            std::string window_title = "MyTowerDefense";
            int window_width = 1280;
            int window_height = 720;
            bool window_resizable = true;
        };
        // 性能设置
        struct PerformanceConfig
        {
            //目标 FPS 设置，0 表示不限制
            int target_fps = 144;
        };
        // 音频设置
        struct AudioConfig
        {
            float music_volume = 0.5f;
            float sound_volume = 0.5f;
        };
        //所有基础配置信息
        struct BasicConfig
        {
            WindowConfig window;
            // 存储动作名称到 SDL Scancode 名称列表的映射
            std::unordered_map<std::string, std::vector<std::string>> input_list =
            {
                // 提供一些合理的默认值，以防配置文件加载失败或缺少此部分
                {"move_left", {"A", "Left"}},
                {"move_right", {"D", "Right"}},
                {"move_up", {"W", "Up"}},
                {"move_down", {"S", "Down"}},
                {"attack", {"J", "MouseLeft"}},
                {"skill", {"K", "MouseRight"}},
                // 可以继续添加更多默认动作
            };
            AudioConfig audio;
            PerformanceConfig performance;
        };
        struct HomeConfig
        {
            int hp = 0;
        };
        struct PlayerConfig
        {
            double speed = 0;
            double normal_attack_interval = 0;
            int normal_attack_damage = 0;
            int skill_damage = 0;
            double skill_interval = 0;
            int skill_recover_speed = 0;
        };
        struct TowerConfig
        {
            double interval[3] = { 0 };
            int damage[3] = { 0 };
            int view_range[3] = { 0 };
            int upgrade_cost[3] = { 0 };
            int cost = 0;
        };
        struct BulletConfig
        {
            double speed = 0;
            bool can_rotated = false;
        };
        struct EnemyConfig
        {
            double speed = 0;
            int hp = 0;
            int damage = 0;
            int reward = 0;
        };
        // 主配置结构
        struct GameConfig
        {
            BasicConfig basic;
            HomeConfig home;
            PlayerConfig player;
            std::unordered_map<std::string, TowerConfig> towers;
            std::unordered_map<std::string, BulletConfig> bullets;
            std::unordered_map<std::string, EnemyConfig> enemies;
        };
    }
     //管理应用程序的配置信息
     //提供配置项的默认值，并支持从 JSON 文件加载
     //如果加载失败或文件不存在，将使用默认值。
    class ConfigManager final 
    {
        using json = nlohmann::json;
    public:
        config::GameConfig config_;
    public:
        ConfigManager(std::string_view);
        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;
        ConfigManager(ConfigManager&&) = delete;
        ConfigManager& operator=(ConfigManager&&) = delete;
        ~ConfigManager() = default;
        private:
        void load(std::string_view);
    };
} // namespace engine::core