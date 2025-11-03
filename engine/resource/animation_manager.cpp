#include "animation_manager.h"
#include"texture_manager.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace engine::resource
{
    void AnimationManager::load(const std::string& filename,TextureManager& texture_manager)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            spdlog::warn("动画管理器配置文件 '{}' 未找到。", filename);
            throw std::runtime_error("Failed to open config file: " + filename);
        }
        json j;
        file >> j;

        //解析动画配置
        const auto& animation_json = j.at("animation");
        for (auto it_a = animation_json.begin(); it_a != animation_json.end(); ++it_a)
        {
            auto& value_json = it_a.value();

            AnimationClip clip;
            //用于分割原图片确定帧序列
            int num_w = 0, num_h = 0;
            std::string texture_path;

            value_json.at("w").get_to(num_w);
            value_json.at("h").get_to(num_h);
            value_json.at("duration").get_to(clip.duration);
            value_json.at("loop").get_to(clip.is_loop);
            value_json.at("texture").get_to(texture_path);

            clip.texture = texture_manager.getTexture(texture_path);
            auto size= texture_manager.getTextureSize(clip.texture);
            SDL_Rect src_rect = { 0 ,0,size.x,size.y};  

            const auto& frame_json = value_json.at("frame");
            for (auto it_f = frame_json.begin(); it_f != frame_json.end(); ++it_f)
            {
                int dir = 3;
                if (it_f.key() == "up")
                    dir = 0;
                else if (it_f.key() == "down")
                    dir = 1;
                else if (it_f.key() == "left")
                    dir = 2;

                auto idx_list = it_f.value().get<std::vector<int>>();
                set_animation_frame(clip.frame[dir],src_rect ,num_w, num_h, idx_list); 
            }
            animation_pool_.emplace(it_a.key(), clip);
        }
    }

    void AnimationManager::set_animation_frame(std::vector<SDL_Rect>& frame, SDL_Rect src_rect, int num_w, int num_h, const std::vector<int>& idx_list)
    {
        int width_frame = src_rect.w / num_w;
        int height_frame = src_rect.h / num_h;
        frame.resize(idx_list.size());
        for (int i = 0; i < idx_list.size(); ++i)
        {
           frame[i].x = (idx_list[i] % num_w) * width_frame;
           frame[i].y = (idx_list[i] / num_w) * height_frame;
           frame[i].w = width_frame;
           frame[i].h = height_frame;
        }
    }
}