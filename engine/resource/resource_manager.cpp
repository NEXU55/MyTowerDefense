#include "resource_manager.h" 
#include"audio_set_manager.h"
#include"animation_manager.h"
#include "texture_manager.h"
#include "audio_manager.h"
#include "font_manager.h" 
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include<filesystem>

namespace engine::resource 
{
    ResourceManager::ResourceManager(SDL_Renderer* renderer) 
    {
        // --- 初始化各个子系统 --- (如果出现错误会抛出异常，由上层捕获)
        audio_set_manager_ = std::make_unique<AudioSetManager>();
        animation_manager_ = std::make_unique<AnimationManager>();
        texture_manager_ = std::make_unique<TextureManager>(renderer);
        audio_manager_ = std::make_unique<AudioManager>();
        font_manager_ = std::make_unique<FontManager>();
        spdlog::trace("ResourceManager 构造成功。");
        // RAII: 构造成功即代表资源管理器可以正常工作，无需再初始化，无需检查指针是否为空
    }

    ResourceManager::~ResourceManager() = default;

    void ResourceManager::clear()
    {
        font_manager_->clear();
        audio_manager_->clear();
        texture_manager_->clear();
        animation_manager_->clear();
        audio_set_manager_->clear();
        spdlog::trace("ResourceManager 中的资源通过 clear() 清空。");
    }

    void ResourceManager::load_all()
    {
        for (const auto& entry : std::filesystem::directory_iterator("resources/texture"))
        {
            load_texture(entry.path().stem().string(), entry.path().string());
        }
        for (const auto& entry : std::filesystem::directory_iterator("resources/music"))
        {
            load_music(entry.path().stem().string(), entry.path().string());
        }
        for (const auto& entry : std::filesystem::directory_iterator("resources/sound"))
        {
            load_sound(entry.path().stem().string(), entry.path().string());
        }
        animation_manager_->load("config/animation_config.json", *texture_manager_);
        audio_set_manager_->load("config/audio_set_config.json");
    }

    std::string_view ResourceManager::get_audio_name(std::string_view type, std::string_view name)
    {
        return audio_set_manager_->get(type,name);
    }

    AnimationClip* ResourceManager::get_animation(std::string_view file_path)
    {
        return animation_manager_->get(file_path);
    }

    void ResourceManager::clear_animation()
    {
        animation_manager_->clear();
    }

    // --- 纹理接口实现 ---
    SDL_Texture* ResourceManager::load_texture(std::string_view name,std::string_view file_path) 
    {
        return texture_manager_->loadTexture(name,file_path);
    }

    SDL_Texture* ResourceManager::get_texture(std::string_view name) 
    {
        return texture_manager_->getTexture(name);
    }

    glm::ivec2 ResourceManager::get_texture_size(std::string_view name)
    {
        return texture_manager_->getTextureSize(name);
    }

    glm::ivec2 ResourceManager::get_texture_size(SDL_Texture* texture)
    {
        return texture_manager_->getTextureSize(texture);
    }

    void ResourceManager::unload_texture(std::string_view name) 
    {
        texture_manager_->unloadTexture(name);
    }

    void ResourceManager::clear_textures() 
    {
        texture_manager_->clear();
    }

    // --- 音频接口实现 ---
    Mix_Chunk* ResourceManager::load_sound(std::string_view name,std::string_view file_path) 
    {
        return audio_manager_->loadSound(name,file_path);
    }

    Mix_Chunk* ResourceManager::get_sound(std::string_view name) 
    {
        return audio_manager_->getSound(name);
    }

    void ResourceManager::unload_sound(std::string_view name) 
    {
        audio_manager_->unloadSound(name);
    }

    void ResourceManager::clear_sounds() {
        audio_manager_->clearSounds();
    }

    Mix_Music* ResourceManager::load_music(std::string_view name,std::string_view file_path) 
    {
        return audio_manager_->loadMusic(name,file_path);
    }

    Mix_Music* ResourceManager::get_music(std::string_view name)
    {
        return audio_manager_->getMusic(name);
    }

    void ResourceManager::unload_music(std::string_view name) 
    {
        audio_manager_->unloadMusic(name);
    }

    void ResourceManager::clear_music() 
    {
        audio_manager_->clearMusic();
    }

    // --- 字体接口实现 ---
    TTF_Font* ResourceManager::load_font(std::string_view file_path, int point_size) 
    {
        return font_manager_->loadFont(file_path, point_size);
    }

    TTF_Font* ResourceManager::get_font(std::string_view name, int point_size) {
        return font_manager_->getFont(name, point_size);
    }

    void ResourceManager::unload_font(std::string_view name, int point_size) {
        font_manager_->unloadFont(name, point_size);
    }

    void ResourceManager::clear_fonts() 
    {
        font_manager_->clear();
    }

} // namespace engine::resource
