#pragma once
#include <memory>       // 用于 std::unique_ptr
#include <string>       // 用于 std::string
#include <string_view>  // 用于 std::string_view
#include <glm/glm.hpp>

// 前向声明 SDL 类型
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct Mix_Chunk Mix_Chunk;
typedef struct _Mix_Music Mix_Music;
typedef struct _TTF_Font TTF_Font;

namespace engine::resource 
{
    // 前向声明内部管理器
    struct AnimationClip;

    class AudioSetManager;
    class AnimationManager;
    class TextureManager;
    class AudioManager;
    class FontManager;

    //在构造时初始化其管理的子系统。构造失败会抛出异常。
    class ResourceManager final 
    {
    private:
        std::unique_ptr<AudioSetManager> audio_set_manager_;
        std::unique_ptr<AnimationManager> animation_manager_;
        std::unique_ptr<TextureManager> texture_manager_;
        std::unique_ptr<AudioManager> audio_manager_;
        std::unique_ptr<FontManager> font_manager_;

    public:
        // explicit 关键字用于防止隐式转换
        explicit ResourceManager(SDL_Renderer* renderer);       
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        ~ResourceManager();  // 显式声明析构函数，这是为了能让智能指针正确管理仅有前向声明的类
        void clear();        //清空所有资源

        // --- 统一资源访问接口 ---
        void load_all();

        //——AudioSet——
        std::string_view get_audio_name(std::string_view type, std::string_view name);

        // ——Animation——
        AnimationClip* get_animation(std::string_view);

        void clear_animation();

        // -- Texture --
        SDL_Texture* load_texture(std::string_view,std::string_view);               //载入纹理资源
        SDL_Texture* get_texture(std::string_view);                                 //尝试获取已加载纹理的指针
        void unload_texture(std::string_view);                                      //卸载指定的纹理资源
        glm::ivec2 get_texture_size(std::string_view);                              //获取指定纹理的尺寸
        glm::ivec2 get_texture_size(SDL_Texture*);
        void clear_textures();                                                      //清空所有纹理资源

        // -- Sound--
        Mix_Chunk* load_sound(std::string_view,std::string_view);                   //载入音效资源
        Mix_Chunk* get_sound(std::string_view);                                     //尝试获取已加载音效的指针
        void unload_sound(std::string_view);                                        //卸载指定的音效资源
        void clear_sounds();                                                        //清空所有音效资源

        // -- Music --
        Mix_Music* load_music(std::string_view,std::string_view);                   //载入音乐资源
        Mix_Music* get_music(std::string_view);                                     //尝试获取已加载音乐的指针
        void unload_music(std::string_view);                                        //卸载指定的音乐资源
        void clear_music();                                                         //清空所有音乐资源

        // -- Fonts --
        TTF_Font* load_font(std::string_view, int);                                 //载入字体资源
        TTF_Font* get_font(std::string_view , int);                                 //尝试获取已加载字体的指针
        void unload_font(std::string_view, int);                                    //卸载指定的字体资源
        void clear_fonts();                                                         //清空所有字体资源
    };
}
