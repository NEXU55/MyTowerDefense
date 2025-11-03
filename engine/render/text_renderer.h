#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <string_view>
#include<SDL.h>

typedef struct _TTF_Font TTF_Font;

namespace engine::resource 
{
    class ResourceManager;
}

namespace engine::render 
{
    class TextRenderer final 
    {
    private:
        SDL_Renderer* sdl_renderer_ = nullptr;                         
        engine::resource::ResourceManager* resource_manager_ = nullptr;
    public:
        /**
         * @brief 构造 TextRenderer。
         * @param sdl_renderer 有效的 SDL_Renderer 指针。
         * @param resource_manager 有效的 ResourceManager 指针（用于字体加载）。
         * @throws std::runtime_error 如果初始化失败。
         */
        TextRenderer(SDL_Renderer* sdl_renderer, engine::resource::ResourceManager* resource_manager);

        //绘制UI上的字符串。
        //text UTF-8 字符串内容。
        //font_id 字体 ID。
        //font_size 字体大小。
        //position 左上角屏幕位置。
        //color 文本颜色。(默认为白色)
        void draw_ui_text(std::string_view text, std::string_view font_id, int font_size,
            const glm::dvec2& position, SDL_Color color = { 1, 1, 1, 1 });
        glm::dvec2 get_text_size(std::string_view text, std::string_view font_id, int font_size);

        // 禁用拷贝和移动语义
        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;
        TextRenderer(TextRenderer&&) = delete;
        TextRenderer& operator=(TextRenderer&&) = delete;
    private:
        void draw_text(std::string_view text, TTF_Font* font, int font_size,glm::ivec2 position, const SDL_Color& color);
    };
}