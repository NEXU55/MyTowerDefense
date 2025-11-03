#include "texture_manager.h"
#include <SDL_image.h>// 用于 IMG_LoadTexture, IMG_Init, IMG_Quit
#include <spdlog/spdlog.h>

namespace engine::resource 
{
    TextureManager::TextureManager(SDL_Renderer* renderer) : renderer_(renderer) 
    {
        //关键错误，无法继续，抛出异常 （它将由catch语句捕获（位于Game），并进行处理）
        if (!renderer_)
            throw std::runtime_error("TextureManager 构造失败: 渲染器指针为空。");
        spdlog::trace("TextureManager 构造成功。");
    }

    SDL_Texture* TextureManager::loadTexture(std::string_view name,std::string_view file_path)
    {
        // 检查是否已加载
        auto it = textures_.find(std::string(name));
        if (it != textures_.end()) 
            return it->second.get();

        // 如果没加载则尝试加载纹理
        SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, file_path.data());

        // 载入纹理时，设置纹理缩放模式为最邻近插值(必不可少，否则TileLayer渲染中会出现边缘空隙/模糊)
        if (SDL_SetTextureScaleMode(raw_texture, SDL_ScaleModeNearest)!=0) 
            spdlog::warn("无法设置纹理缩放模式为最邻近插值");

        if (!raw_texture) 
        {
            spdlog::error("加载纹理失败: '{}': {}", file_path, SDL_GetError());
            return nullptr;
        }

        // 使用带有自定义删除器的 unique_ptr 存储加载的纹理
        textures_.emplace(name, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(raw_texture));
        spdlog::debug("成功加载并缓存纹理: {}", name);

        return raw_texture;
    }

    SDL_Texture* TextureManager::getTexture(std::string_view name) 
    {
        // 查找现有纹理
        auto it = textures_.find(std::string(name));
        if (it != textures_.end())
            return it->second.get();
        spdlog::warn("纹理 '{}' 未找到缓存。", name);
        return nullptr;
    }

    glm::ivec2 TextureManager::getTextureSize(std::string_view name) 
    {
        glm::ivec2 size(0);
        // 获取纹理
        SDL_Texture* texture = getTexture(name);
        if (!texture) {
            spdlog::error("无法获取纹理: {}", name);
            return size;
        }

        // 获取纹理尺寸
        int width = 0, height = 0;
        if (SDL_QueryTexture(texture,NULL,NULL,&width,&height)!= 0)
        {
            spdlog::error("无法查询纹理尺寸: {}", name);
            return size;
        }
        size.x = width;
        size.y = height;
        return size;
    }

    glm::ivec2 TextureManager::getTextureSize(SDL_Texture* texture)
    {
        glm::ivec2 size(0);
        // 获取纹理尺寸
        int width = 0, height = 0;
        if (SDL_QueryTexture(texture, NULL, NULL, &width, &height) != 0)
        {
            spdlog::error("无法通过该指针查询纹理尺寸");
            return size;
        }
        size.x = width;
        size.y = height;
        return size;
    }

    void TextureManager::unloadTexture(std::string_view name)
    {
        auto it = textures_.find(std::string(name));
        if (it != textures_.end()) 
        {
            spdlog::debug("卸载纹理: {}", name);
            textures_.erase(it);// unique_ptr 通过自定义删除器处理删除
        } 
        else
            spdlog::warn("尝试卸载不存在的纹理: {}", name);
    }

    void TextureManager::clear() 
    {
        if (!textures_.empty()) 
        {
            spdlog::debug("正在清除所有 {} 个缓存的纹理。", textures_.size());
            textures_.clear(); // unique_ptr 处理所有元素的删除
        }
    }
}
