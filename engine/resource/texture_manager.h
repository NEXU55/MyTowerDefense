#pragma once
#include <memory>       // 用于 std::unique_ptr
#include <stdexcept>    // 用于 std::runtime_error
#include <string>       // 用于 std::string
#include <string_view> // 用于 std::string_view
#include <unordered_map> // 用于 std::unordered_map
#include <SDL_render.h> // 用于 SDL_Texture 和 SDL_Renderer
#include <glm/glm.hpp>

namespace engine::resource 
{
     //管理 SDL_Texture 资源的加载、存储和检索。
     //在构造时初始化。使用文件路径作为键，确保纹理只加载一次并正确释放。
     //依赖于一个有效的 SDL_Renderer，构造失败会抛出异常。
    class TextureManager final
    {
        friend class ResourceManager;
        friend class AnimationManager;
    private:
        // SDL_Texture 的删除器函数对象，用于智能指针管理
        struct SDLTextureDeleter 
        {
            void operator()(SDL_Texture* texture)
            {
                if (texture) 
                    SDL_DestroyTexture(texture);
            }
        };
        // 存储文件路径和指向管理纹理的 unique_ptr 的映射
        std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> textures_;
        SDL_Renderer* renderer_ = nullptr; // 指向主渲染器的非拥有指针
    public:
        //renderer 指向有效的 SDL_Renderer 上下文的指针。不能为空
        //std::runtime_error 如果 renderer 为 nullptr 或初始化失败。

        explicit TextureManager(SDL_Renderer* renderer);
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;
        ~TextureManager() = default;
    private: // 仅供 ResourceManager 访问的方法
        SDL_Texture* loadTexture(std::string_view,std::string_view);      //从文件路径加载纹理
        SDL_Texture* getTexture(std::string_view);       //尝试获取已加载纹理的指针
        glm::ivec2 getTextureSize(std::string_view);      //获取指定纹理的尺寸
        glm::ivec2 getTextureSize(SDL_Texture*);
        void unloadTexture(std::string_view);            //卸载指定的纹理资源
        void clear();                                              //清空所有纹理资源
    };
}
