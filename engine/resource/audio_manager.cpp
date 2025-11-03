#include "audio_manager.h"
#include <spdlog/spdlog.h>

namespace engine::resource 
{
// --- 音效管理 ---
Mix_Chunk* AudioManager::loadSound(std::string_view name,std::string_view file_path)
{
    // 首先检查缓存
    auto it = sounds_.find(std::string(name));
    if (it != sounds_.end()) 
        return it->second.get();

    // 加载音效块
    spdlog::debug("加载音效: {}", file_path);
    Mix_Chunk* raw_chunk = Mix_LoadWAV(file_path.data());
    if (!raw_chunk) 
    {
        spdlog::error("加载音效失败: '{}': {}", file_path, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    sounds_.emplace(name, std::unique_ptr<Mix_Chunk, SDLMixChunkDeleter>(raw_chunk));
    spdlog::debug("成功加载并缓存音效: {}", name);
    return raw_chunk;
}

Mix_Chunk* AudioManager::getSound(std::string_view name)
{
    auto it = sounds_.find(std::string(name));
    if (it != sounds_.end())
        return it->second.get();
    spdlog::warn("音效 '{}' 未找到缓存。", name);
    return nullptr;
}

void AudioManager::unloadSound(std::string_view name)
{
    auto it = sounds_.find(std::string(name));
    if (it != sounds_.end()) 
    {
        spdlog::debug("卸载音效: {}", name);
        sounds_.erase(it);      // unique_ptr处理Mix_FreeChunk
    } 
    else 
        spdlog::warn("尝试卸载不存在的音效: {}", name);
}

void AudioManager::clearSounds() 
{
    if (!sounds_.empty()) 
    {
        spdlog::debug("正在清除所有 {} 个缓存的音效。", sounds_.size());
        sounds_.clear(); // unique_ptr处理删除
    }
}

// --- 音乐管理 ---
Mix_Music* AudioManager::loadMusic(std::string_view name,std::string_view file_path)
{
    // 首先检查缓存
    auto it = music_.find(std::string(name));
    if (it != music_.end()) 
        return it->second.get();

    // 加载音乐
    spdlog::debug("加载音乐: {}", file_path);
    Mix_Music* raw_music = Mix_LoadMUS(file_path.data());
    if (!raw_music) 
    {
        spdlog::error("加载音乐失败: '{}': {}", file_path, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    music_.emplace(name, std::unique_ptr<Mix_Music, SDLMixMusicDeleter>(raw_music));
    spdlog::debug("成功加载并缓存音乐: {}", name);
    return raw_music;
}

Mix_Music* AudioManager::getMusic(std::string_view name)
{
    auto it = music_.find(std::string(name));
    if (it != music_.end())
        return it->second.get();
    spdlog::warn("音乐 '{}' 未找到缓存。", name);
    return nullptr;
}

void AudioManager::unloadMusic(std::string_view name)
{
    auto it = music_.find(std::string(name));
    if (it != music_.end()) 
    {
        spdlog::debug("卸载音乐: {}", name);
        music_.erase(it); // unique_ptr处理Mix_FreeMusic
    } 
    else
        spdlog::warn("尝试卸载不存在的音乐: {}", name);
}

void AudioManager::clearMusic() 
{
    if (!music_.empty()) 
    {
        spdlog::debug("正在清除所有 {} 个缓存的音乐曲目。", music_.size());
        music_.clear(); 
    }
}

void AudioManager::clear()
{
    // 立即停止所有音频播放
    Mix_HaltChannel(-1); // 停止所有音效
    Mix_HaltMusic();     // 停止音乐
    clearSounds();
    clearMusic();
}
} // namespace engine::resource
