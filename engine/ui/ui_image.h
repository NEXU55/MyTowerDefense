#pragma once
#include "ui_element.h"
#include <SDL_rect.h>

typedef struct SDL_Texture SDL_Texture;

namespace engine::ui 
{
    /*
     * 一个用于显示纹理或部分纹理的UI元素。
     * 继承自UIElement并添加了渲染图像的功能。
     */
    class UIImage final : public UIElement 
    {
    protected:
        SDL_Texture* texture_ = nullptr;           //纹理资源
        SDL_Rect src_rect_ = { 0 };                //要绘制的纹理部分
    public:
        UIImage(SDL_Texture* texture,glm::dvec2 position,
            glm::dvec2 size,SDL_Rect src_rect);
        // --- 核心方法 ---
        void render(engine::core::Context& context) override;
        // --- Setters & Getters ---
        SDL_Texture* get_texture() const { return texture_; }
        void set_texture(SDL_Texture* texture) { texture_ = texture; }

        const SDL_Rect& get_src_rect() const { return src_rect_; }
        void set_src_rect(SDL_Rect src_rect) { src_rect_=std::move(src_rect); }
    };
}