#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace engine::core 
{
    namespace config
    {
        // JSON 解析函数
        static void from_json(const json& j, WindowConfig& w)
        {
            j.at("title").get_to(w.window_title);
            j.at("width").get_to(w.window_width);
            j.at("height").get_to(w.window_height);
        }
        static void from_json(const json& j, PerformanceConfig& p)
        {
            j.at("target_fps").get_to(p.target_fps);
        }
        static void from_json(const json& j, AudioConfig& a)
        {
            j.at("music_volume").get_to(a.music_volume);
            j.at("sound_volume").get_to(a.sound_volume);
        }
        static void from_json(const json& j, BasicConfig& b)
        {
            j.at("window").get_to(b.window);
            j.at("performance").get_to(b.performance);
            j.at("audio").get_to(b.audio);
            // 从 JSON 加载 input_list
            const auto& input_json = j.at("input_list");
            try// 直接尝试从 JSON 对象转换为 unordered_map<string, vector<string>>
            {
                auto input_list = input_json.get<std::unordered_map<std::string, std::vector<std::string>>>();
                // 如果成功，则将 input_mappings 移动到 input_mappings_
                b.input_list = std::move(input_list);
                spdlog::trace("成功从配置加载输入映射。");
            }
            catch (const std::exception& e) 
            {
                spdlog::warn("配置加载警告：解析 'input_lsit' 时发生异常。使用默认映射。错误：{}", e.what());
            }
        }
        static void from_json(const json& j, HomeConfig& h)
        {
            j.at("hp").get_to(h.hp);
        }
        static void from_json(const json& j, PlayerConfig& p)
        {
            j.at("speed").get_to(p.speed);
            j.at("normal_attack_interval").get_to(p.normal_attack_interval);
            j.at("normal_attack_damage").get_to(p.normal_attack_damage);
            j.at("skill_interval").get_to(p.skill_interval);
            j.at("skill_damage").get_to(p.skill_damage);
            j.at("skill_recover_speed").get_to(p.skill_recover_speed);
        }
        static void from_json(const json& j, TowerConfig& t)
        {
            j.at("interval").get_to(t.interval);
            j.at("damage").get_to(t.damage);
            j.at("view_range").get_to(t.view_range);
            j.at("cost").get_to(t.cost);
            j.at("upgrade_cost").get_to(t.upgrade_cost);
        }
        static void from_json(const json& j, BulletConfig& b)
        {
            j.at("speed").get_to(b.speed);
            j.at("can_roatated").get_to(b.can_rotated);
        }
        static void from_json(const json& j, EnemyConfig& e)
        {
            j.at("hp").get_to(e.hp);
            j.at("speed").get_to(e.speed);
            j.at("damage").get_to(e.damage);
            j.at("reward").get_to(e.reward);
        }
    }
    
    ConfigManager::ConfigManager(std::string_view path)
    {
        load(path);
    }

    void ConfigManager::load(std::string_view path)
    {
        std::ifstream file(path.data());
        if (!file.is_open()) 
        {
            spdlog::warn("配置文件 '{}' 未找到。使用默认设置。", path);
            throw std::runtime_error("Failed to open config file: " + std::string(path));
        }
        json j;
        file >> j;

        // 解析基本配置
        try
        {
            j.at("basic").get_to(config_.basic);
        }
        catch (const std::exception& e)
        {
            spdlog::warn("读取基本配置失败，{}",e.what());
        }
        // 解析家配置
        try
        {
            j.at("home").get_to(config_.home);
        }
        catch (const std::exception& e)
        {
            spdlog::warn("读取基本配置失败，{}", e.what());
        }
        // 解析玩家配置
        try
        {
            j.at("player").get_to(config_.player);
        }
        catch (const std::exception& e)
        {
            spdlog::warn("读取玩家配置失败，{}", e.what());
        }
        // 解析塔配置
        try
        {
            const auto& tower_json = j.at("tower");
            for (auto it = tower_json.begin(); it != tower_json.end(); ++it)
            {
                config::TowerConfig tower_config;
                it.value().get_to(tower_config);
                config_.towers[it.key()] = tower_config;
            }
        }
        catch (const std::exception& e)
        {
            spdlog::warn("读取防御塔配置失败，{}", e.what());
        }
        
        //解析子弹配置
        try
        {
            const auto& bullet_json = j.at("bullet");
            for (auto it = bullet_json.begin(); it != bullet_json.end(); ++it)
            {
                config::BulletConfig bullet_config;
                it.value().get_to(bullet_config);
                config_.bullets[it.key()] = bullet_config;
            }
        }
        catch (const std::exception& e)
        {
            spdlog::warn("读取子弹配置失败，{}", e.what());
        }
        
        // 解析敌人配置
        try
        {
            const auto& enemy_json = j.at("enemy");
            for (auto it = enemy_json.begin(); it != enemy_json.end(); ++it)
            {
                config::EnemyConfig enemy_config;
                it.value().get_to(enemy_config);
                config_.enemies[it.key()] = enemy_config;
            }
        }
        catch (const std::exception& e)
        {
            spdlog::warn("读取敌人配置失败，{}", e.what());
        }
    }
    
} // namespace engine::core 