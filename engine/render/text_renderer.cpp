#include "../resource/resource_manager.h"
#include "text_renderer.h"
#include <SDL_ttf.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::render {

    TextRenderer::TextRenderer(SDL_Renderer* sdl_renderer, engine::resource::ResourceManager* resource_manager)
        : sdl_renderer_(sdl_renderer),
        resource_manager_(resource_manager)
    {
        if (!sdl_renderer_ || !resource_manager_) 
            throw std::runtime_error("TextRenderer 需要一个有效的 SDL_Renderer 和 ResourceManager。");
        spdlog::trace("TextRenderer 初始化成功.");
    }

    void TextRenderer::draw_ui_text(std::string_view text, std::string_view font_id, int font_size,
        const glm::dvec2& position, SDL_Color color)
    {
        /* 构造函数已经保证了必要指针不会为空，这里不需要再检查 */
        TTF_Font* font = resource_manager_->get_font(font_id, font_size);
        if (!font) 
        {
            spdlog::warn("drawUIText 获取字体失败: {} 大小 {}", font_id, font_size);
            return;
        } 
        // 先渲染一次黑色文字模拟阴影
        draw_text(text, font, font_size, position + glm::dvec2{2,2}, { 175,175,175,255 });
        // 然后正常绘制
        draw_text(text, font , font_size ,position, color);
    }

    glm::dvec2 TextRenderer::get_text_size(std::string_view text, std::string_view font_id, int font_size) 
    {
        glm::dvec2 size{0,0};
        TTF_Font* font = resource_manager_->get_font(font_id, font_size);
        if (!font) {
            spdlog::warn("get_text_size 获取字体失败: {} 大小 {}", font_id, font_size);
            return size;
        }
        // 创建表面
        SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.data(), {0,0,0,0});
        if (!text_surface)
        {
            spdlog::error("get_text_size 创建 text_surface 失败: {}", SDL_GetError());
            return size;
        }
        size = { text_surface->w, text_surface->h };
        SDL_FreeSurface(text_surface);
        return size;
    }

    void TextRenderer::draw_text(std::string_view text,TTF_Font* font , int font_size,
        glm::ivec2 position, const SDL_Color& color)
    {
        // 创建表面
        SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.data(), color);
        if (!text_surface)
        {
            spdlog::error("draw_ui_text 创建 text_surface 失败: {}", SDL_GetError());
            return;
        }
        // 从表面创建纹理
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(sdl_renderer_, text_surface);
        if (!text_texture)
        {
            SDL_FreeSurface(text_surface);
            spdlog::error("Unable to create texture from rendered text:{} ", SDL_GetError());
        }
        // 设置渲染位置
        SDL_Rect render_dst = { position.x, position.y, text_surface->w, text_surface->h };
        // 渲染到屏幕
        SDL_RenderCopy(sdl_renderer_, text_texture, nullptr, &render_dst);
        // 清理资源
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}