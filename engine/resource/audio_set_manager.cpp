#include "audio_set_manager.h"
#include<spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>

namespace engine::resource
{
    void AudioSetManager::load(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            spdlog::warn("音乐集管理器配置文件 '{}' 未找到。", filename);
            throw std::runtime_error("Failed to open config file: " + filename);
        }
        json j;
        file >> j;

        //解析动画配置
        const auto& audio_set_json = j.at("audio_set");
        for (auto it_k = audio_set_json.begin(); it_k != audio_set_json.end(); ++it_k)
        {
            const auto& value_json = it_k.value();
            for (auto it_v = value_json.begin(); it_v != value_json.end(); ++it_v)
            {
                audio_set_pool_[it_k.key()][it_v.key()]= it_v.value().get<std::vector<std::string>>();
            }
        }
    }
}